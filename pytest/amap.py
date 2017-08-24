# encoding:utf-8

import os
from flask import g
from flask import json
from flask import abort
from flask import Flask
from flask import jsonify
from flask import url_for
from flask import session
from flask import request
from flask import render_template
from flask import send_from_directory


app = Flask(__name__)


@app.route('/')
def root():
    return render_template('index.html')


@app.route('/index.html')
def index():
    return render_template('index.html')


@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                               'favicon.ico', mimetype='image/vnd.microsoft.icon')


@app.route("/", methods=['GET'])
def hello():
    rv = 'hello, flask!'
    return rv 


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8000)

