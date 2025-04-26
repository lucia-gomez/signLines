from flask import Flask, request, jsonify
from flask_cors import CORS
from pyaxidraw import axidraw

import base64
import os
import serial
import serial.tools.list_ports
import tempfile
import time

PORT = 8080
app = Flask(__name__)
CORS(app)

LOG_TAG = "[server]"
ENABLE_LOGGING = True

def info(msg):
	if ENABLE_LOGGING:
		print(f"{LOG_TAG} {msg}", flush=True)


def find_arduino():
		ports = serial.tools.list_ports.comports()
		for port in ports:
				if 'Arduino' in port.description:
						return port.device
		return None


@app.route('/', methods=['GET'])
def ping():
	return 'Success', 200


def prime_color(i, enable):
	msg = f"PRIME:{i}:{int(enable)}"
	serial_write(msg)


@app.route('/prime', methods=['POST'])
def prime_endpoint():
	try:
			payload = request.get_json()
			index = payload.get("index")
			enable = payload.get("enable")
			if not isinstance(enable, bool) or not isinstance(index, int):
				raise ValueError("Invalid payload")
			
			prime_color(index, enable)
			return 'Success', 200
	except Exception as e:
		print(f"Error parsing body: {e}")
		return jsonify({"error": "Invalid payload"}), 400


def water(enable):
	prime_color(6, enable)
	

@app.route('/water', methods=['POST'])
def water_endpoint():
	try:
			payload = request.get_json()
			enable = payload.get("enable")
			if not isinstance(enable, bool):
				raise ValueError("Invalid payload: must enable/disable water")
			
			water(enable)
			return 'Success', 200
	except Exception as e:
		print(f"Error parsing body: {e}")
		return jsonify({"error": "Invalid payload"}), 400


def set_color(i):
	msg = f"COLOR:{i}"
	serial_write(msg)

	
def set_draw_mode(enable=False):
	msg = f"DRAW:{int(enable)}:"
	serial_write(msg)


def serial_write(msg):
	if not arduino_port:
		raise Exception("Arduino not found. Check connection")
	
	with serial.Serial(arduino_port, 9600, timeout=1) as ser:
		ser.write(msg.encode())
		info(f"Sent: {msg} to {arduino_port}")


ad = axidraw.AxiDraw()
arduino_port = find_arduino()


@app.route('/complete', methods=['POST'])
def complete():
		try:
			payload = request.get_json()
			color = payload.get("color")
			if not isinstance(color, str):
				raise ValueError("Invalid payload:  must specify color")
			
			on_plot_complete(color)
			return 'Success', 200
		except Exception as e:
			print(f"Error parsing body: {e}")
			return jsonify({"error": "Invalid payload"}), 400
	

@app.route('/start', methods=['POST'])
def start():
		try:
				payload = request.get_json()
				svg_base64 = payload.get('file')
				color_idx = payload.get("colorIdx")
				
				if not isinstance(svg_base64, str) or not isinstance(color_idx, int):
						raise ValueError("Invalid payload")
				
				svg_data = base64.b64decode(svg_base64)
				with tempfile.NamedTemporaryFile(delete=False, suffix='.svg') as temp_file:
					temp_file.write(svg_data)
					temp_file_path = temp_file.name
				
				on_start_plot(temp_file_path, color_idx)
				os.remove(temp_file_path)
				return 'Success', 200
		except Exception as e:
				print(f"Error parsing body: {e}")
				return jsonify({"error": "Invalid payload"}), 400


@app.route('/off', methods=['POST'])
def off():
	ad.plot_setup()
	ad.options.mode = "manual"
	ad.options.manual_cmd = "disable_xy"
	ad.plot_run()
	return 'Success', 200
		

def on_start_plot(filename, color_idx):
	info(f"Plotting {filename}...")
	ad.params.use_b3_out = True
	
	try:
		time.sleep(0.5)
		prime_color(color_idx, True)
		time.sleep(2 if color_idx == 1 or color_idx == 2 else 1)
		prime_color(color_idx, False)
		# set_draw_mode(True)
		time.sleep(2)
		ad.plot_setup(filename)
		ad.options.pen_pos_up = 90
		ad.options.pen_pos_down = 50 
		ad.plot_run()
		ad.disconnect()
		# set_draw_mode(False)
	except Exception as ex:
		print(ex)


def on_plot_complete(color):
	info(f"Finished plotting {color}")
	time.sleep(1)
	water(True)
	time.sleep(3)
	water(False)
	time.sleep(1.5)
	ad.plot_setup()
	ad.options.mode = "manual"
	ad.options.manual_cmd = "disable_xy"
	ad.plot_run()
	return

if __name__ == '__main__':
		app.run(host='0.0.0.0', port=PORT, debug=True, threaded=True)
