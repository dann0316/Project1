import processing.serial.*;

Serial serialPort;
PrintWriter outputFile;

void setup() {
  String[] ports = Serial.list();
  if (ports.length == 0) {
    println("No serial ports found!");
    exit();
  }

  String portName = "COM1"; // Modify this to match the Arduino serial port
  serialPort = new Serial(this, portName, 115200);
  serialPort.bufferUntil('\n'); // Wait for a newline character to receive data

  // Create the output file on the desktop
  String desktopPath = System.getProperty("user.home") + "/Desktop/";
  String fileName = "data.txt";
  outputFile = createWriter(desktopPath + fileName);
  if (outputFile == null) {
    println("Failed to create the output file!");
    exit();
  }

  // Write the column headers to the file
  outputFile.println("Roll(degrees)\tPitch(degrees)\tTemperature(degrees Celsius)");
}

void draw() {
  // Empty draw() function since we don't need continuous drawing
}

void serialEvent(Serial serial) {
  while (serial.available() > 0) {
    String data = serial.readStringUntil('\n');
    if (data != null) {
      data = data.trim();
      String[] values = data.split("\t");
      if (values.length == 3) {
        float roll = float(values[0]);
        float pitch = float(values[1]);
        float temperature = float(values[2]);

        // Write the data to the output file
        outputFile.print(roll);
        outputFile.print("\t");
        outputFile.print(pitch);
        outputFile.print("\t");
        outputFile.println(temperature);
        outputFile.flush(); // Flush the data to ensure it's written to the file
      }
    }
  }
}

void keyPressed() {
  if (key == 's' || key == 'S') {
    outputFile.flush(); // Flush the data to ensure it's written to the file
    outputFile.close(); // Close the file
    println("Data saved successfully!");
    exit(); // Quit the Processing program
  }
}
