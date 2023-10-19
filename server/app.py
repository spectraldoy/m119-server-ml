from flask import Flask, request

app = Flask(__name__)
app.config["SECRET_KEY"] = "secret!"

@app.route("/")
def hello():
    return "Hello!"

@app.route("/query", methods=["GET"])
def inference():
    return request.args
