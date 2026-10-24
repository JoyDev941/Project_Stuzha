extends Control
@onready var drive_dropdown: OptionButton = $_drives_panel/DriveDropdown

var available_drives: Array[Dictionary] = []


func _ready():
	detect_drives()


func detect_drives():
	drive_dropdown.clear()
	available_drives.clear()

	var output: Array = []

	var exit_code = OS.execute(
		"lsblk",
		[
			"--json",
			"--bytes",
			"--output", "NAME,PATH,SIZE,TYPE,RM,MODEL"
		],
		output
	)

	if exit_code != 0 or output.is_empty():
		drive_dropdown.add_item("Failed to detect drives")
		drive_dropdown.set_item_disabled(0, true)
		return

	var result = JSON.parse_string(output[0])

	if not result is Dictionary:
		drive_dropdown.add_item("Invalid drive information")
		drive_dropdown.set_item_disabled(0, true)
		return

	for device in result.get("blockdevices", []):
		if device.get("type") != "disk":
			continue

		if not device.get("rm", false):
			continue

		var drive: Dictionary = {
			"path": device.get("path", ""),
			"name": device.get("name", "Unknown"),
			"size": int(device.get("size", 0)),
			"model": device.get("model", "Unknown device")
		}

		available_drives.append(drive)

		var size_gb = drive["size"] / 1_000_000_000.0

		var display_name = "%s — %.2f GB — %s" % [
			drive["path"],
			size_gb,
			drive["model"]
		]

		drive_dropdown.add_item(display_name)
		drive_dropdown.set_item_metadata(
			drive_dropdown.item_count - 1,
			available_drives.size() - 1
		)

	if available_drives.is_empty():
		drive_dropdown.add_item("No removable drives detected")
		drive_dropdown.set_item_disabled(0, true)
		return

	drive_dropdown.select(0)


func get_selected_drive() -> Dictionary:
	var index = drive_dropdown.selected

	if index < 0:
		return {}

	var drive_index = drive_dropdown.get_item_metadata(index)

	if typeof(drive_index) != TYPE_INT:
		return {}

	if drive_index < 0 or drive_index >= available_drives.size():
		return {}

	return available_drives[drive_index]


func _on_drive_dropdown_item_selected(index: int):
	var drive = get_selected_drive()

	if drive.is_empty():
		return

	print("Selected drive: ", drive["path"])
	print("Model: ", drive["model"])
	print("Capacity: ", drive["size"], " bytes")
