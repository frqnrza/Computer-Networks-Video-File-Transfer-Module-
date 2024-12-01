# <p align="center"> ***File Transfer over TCP with GTK-Based GUI***
This project implements a file transfer system over TCP sockets using a GTK-based graphical user interface (GUI). The system consists of two programs:

* ### **Sender :** Allows the user to select a file and send it to the receiver over a network.
* ### **Receiver :** Receives the file, tracks transfer progress, and allows the user to save the received file to a custom location.

# <p align="center"> **Features** : 
### **Sender** :  
* Select a file using a file chooser.
* Enter the receiver's IP address for connection.
* Tracks and displays the progress of file transfer with a percentage below the progress bar.
* Provides feedback on the file transfer status (e.g., "Sending file...", "File sent successfully!").
### Receiver : 
* Listens for incoming file transfers on a specified port.
* Tracks the progress of the received file with a percentage below the progress bar.
* Allows the user to select a save path for the received file via a file chooser.
* Provides feedback on the transfer status (e.g., "Waiting for sender...", "Receiving file...", "Video received successfully!").

# <p align="center"> Technologies Used : 
* C Programming Language
* GTK+ 3 for GUI
* POSIX Socket Programming for network communication

# <p align="center"> How to Run
### 1. Prerequisites :
Ensure the following are installed on your system :
* Linux (Ubunto OS)
* GCC Compiler :
  
  `-sudo apt-get update`

  `-sudo apt-get install gcc`
* GTK+ development libraries :
  
  `sudo apt-get update`

  `sudo apt-get install libgtk-3-dev`

### 2. Compilation : 
* Use the gcc compiler to compile the sender and receiver programs.

Sender : 

```gcc sender.c -o sender pkg-config --cflags --libs gtk+-3.0```

Receiver :

```gcc receiver.c -o receiver pkg-config --cflags --libs gtk+-3.0```

### 3. Execution : 

* Step 1 :
  Run the Receiver. Open a terminal and start the receiver program with following command :
  
  ```./receiver```

    -The receiver GUI will open , displaying a "Start Receiving" button. Click it to start listening for incoming connections.

* Step 2 :
  Run the Sender. Open another terminal and start the sender program with the following command :

  ```./sender```

The sender GUI will open, allowing you to :

     * Select a file using the file chooser.
     * Enter the receiver's IP address (e.g., 127.0.0.1 for localhost or the receiver's actual IP if on a network).
     * Click the "Send File" button to start the transfer.

# <p align="center"> GUI (Graphical User Interface)  

### Sender GUI : 

1. **File Selection :** Provides a dropdown to select the file to send.

2. **Transfer Progress :** Displays a progress bar and percentage below it.

3. **Feedback :** Status updates like "Sending file..." or "File sent successfully!"

### Receiver GUI : 

1. **Listening for File :** Status label shows "Waiting for sender...".

2. **Progress Tracking:** Dynamically updated progress bar and percentage.

3. **Save Path Selection:** File chooser allows the user to select where to save the file after it is received.

4. **Feedback:** Status updates like "Receiving file..." or "Video received successfully!".

# <p align="center"> Code Overview :  

### 1. Sender Code

**Core Functionality:**

* Establishes a TCP connection to the receiver.
* Sends the file in chunks.
* Updates the progress bar and percentage dynamically during the transfer.
* Notifies the receiver of the end of the file using an "EOF" marker.

**Key Functions :**

`send_file()`: Handles file transmission and progress tracking.

`on_send_clicked()`: Triggered when the user clicks the "Send File" button.

### 2. Receiver Code

**Core Functionality:**

* Sets up a TCP server to listen for incoming file transfers.
* Receives the file in chunks and writes it to a temporary location.
* Allows the user to select a save path for the file after the transfer.

**Key Functions:** 

`receive_file()`: Handles file reception and updates the progress bar.

`save_file()`: Allows the user to save the received file to the chosen location.

# <p align="center"> Usage :

**Networking Setup:**
* For local testing, use 127.0.0.1 as the receiver's IP address.
* For testing over a network, ensure both machines are on the same network, and use the receiver's actual IP address (e.g., 192.168.x.x).

# <p align="center"> Error Handling:
If the GUI displays errors like "Socket creation failed!" or "Connection failed!":
* Check if the receiver is running.
* Verify the network connection and IP address.

# <p align="center"> Future Enhancements : 
* Encryption: Add file encryption for secure transfers.
* Multiple Connections: Allow the receiver to handle multiple senders simultaneously.
* Cross-Platform Support: Extend functionality to other operating systems like Windows and macOS.
