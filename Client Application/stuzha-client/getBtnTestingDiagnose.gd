extends Node2D

@onready var http = $HTTPRequest
@onready var button = $Button
@onready var output = $RichTextLabel

func _ready():
	button.pressed.connect(send_request)
	http.request_completed.connect(_on_request_completed)

func send_request():
	http.request("http://192.168.68.108/diagnose")

func _on_request_completed(result, code, headers, body):
	output.text = body.get_string_from_utf8()
