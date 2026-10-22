extends Node

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	detect_drives()
	
func detect_drives():
	var output: Array = []
	
	var exit_code = OS.execute(
		"lsblk",
		["--json", "--bytes", "--output", "NAME,PATH,SIZE,TYPE,FSTYPE,MODEL"],
		output
	)
	
	if exit_code == 0:
		print("Detected Drives: ")
		print(output[0])
	else:
		print("No Drives Detected")
