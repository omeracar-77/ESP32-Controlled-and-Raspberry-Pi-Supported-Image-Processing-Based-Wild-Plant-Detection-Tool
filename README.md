Project Summary: As part of my undergraduate thesis project at Konya Technical University, I designed and developed an autonomous and mobile
robotic system capable of detecting weeds in agricultural fields in real time.

👨‍💻 My Technical Approach and Contributions

Hybrid Hardware Architecture: I built a task-distributed structure to balance the processing load. While managing low-level hardware
controls (Motors, Pan-Tilt, HC-SR04 Sensors) with ESP32, I ran artificial intelligence and image processing processes on Raspberry Pi 5.

Artificial Intelligence and Image Processing: I trained a deep learning model based on CNN (Convolutional Neural Network) using a dataset
consisting of 13 different plant classes and 5500+ images. By using image preprocessing techniques, I increased the model's success rate in field
conditions to over 85%.

Kaggle Dataset: Plant Seedlings Classification
https://www.kaggle.com/competitions/plant-seedlings-classification

IoT and Remote Control: I developed a responsive web interface based on Flask so that the user can control the system via their
mobile phone or computer. Using the Wi-Fi capabilities of the ESP32, I set up an IoT infrastructure that provides live image transmission and real-time data tracking.

🛠️ Technologies Used

Hardware: Raspberry Pi 5, ESP32, L298N Motor Driver, Pan-Tilt Kit, HC-SR04, USB Web Camera.

Software: Python (TensorFlow/Keras, OpenCV, Flask), C++.
