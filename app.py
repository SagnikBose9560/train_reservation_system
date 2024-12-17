from flask import Flask, request, jsonify
import subprocess
import os

app = Flask(__name__)

# Paths
BACKEND_EXECUTABLE = "./backend/backend"
INPUT_FILE = "./backend/input.txt"
OUTPUT_FILE = "./backend/output.txt"

@app.route("/login", methods=["POST"])
def login():
    data = request.get_json()
    username = data.get("username")
    password = data.get("password")

    # Write to input.txt
    with open(INPUT_FILE, "w") as f:
        f.write(f"LOGIN\n{username} {password}\n")

    # Run the backend executable
    run_backend()

    # Read the output
    result = read_output()

    return jsonify({"message": result})


@app.route("/register", methods=["POST"])
def register():
    data = request.get_json()
    username = data.get("username")
    password = data.get("password")

    # Write to input.txt
    with open(INPUT_FILE, "w") as f:
        f.write(f"REGISTER\n{username} {password}\n")

    # Run the backend executable
    run_backend()

    # Read the output
    result = read_output()

    return jsonify({"message": result})


@app.route("/view_trains", methods=["GET"])
def view_trains():
    # Write to input.txt
    with open(INPUT_FILE, "w") as f:
        f.write("VIEW_TRAINS\n")

    # Run the backend executable
    run_backend()

    # Read the output
    result = read_output()

    return jsonify({"trains": result})


def run_backend():
    """Executes the backend C program."""
    try:
        subprocess.run([BACKEND_EXECUTABLE], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running backend: {e}")


def read_output():
    """Reads the output from the output.txt file."""
    if not os.path.exists(OUTPUT_FILE):
        return "Output file not found."

    with open(OUTPUT_FILE, "r") as f:
        return f.read().strip()


if __name__ == "__main__":
    app.run(debug=True)
