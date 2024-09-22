# Aquacare-Iot

AQUACARE is a web-based dashboard that displays real-time environmental and water-related data collected from various sensors. It allows users to monitor important parameters such as ambient temperature, humidity, light level, distance to water, and water temperature. The data is fetched from a Firebase Realtime Database, making it easy to track changes dynamically.

## Features

- **Ambient Temperature**: Displays the current ambient temperature in Celsius.
- **Ambient Humidity**: Displays the current ambient humidity as a percentage.
- **Light Level**: Shows the current light level as a percentage (indicating darkness).
- **Distance to Water**: Displays the distance to water in centimeters using an ultrasonic sensor.
- **Water Temperature**: Shows the current water temperature in Celsius.

## Project Structure

- **HTML & CSS**: The layout and styling of the dashboard are handled with HTML and CSS, utilizing the W3.CSS library for a responsive design.
- **JavaScript**: JavaScript is used to fetch data from Firebase and update the dashboard in real time.
- **Firebase**: Firebase Realtime Database is used to store and retrieve data collected from sensors. It enables real-time updates to the dashboard.
  
## Technologies Used

- **HTML5 & CSS3**: For the structure and layout of the webpage.
- **JavaScript**: For dynamic interaction and data fetching from Firebase.
- **Firebase Realtime Database**: For storing and retrieving real-time sensor data.
- **W3.CSS**: For responsive styling and layout.

## Setup and Installation

1. Clone this repository to your local machine.
   ```bash
   git clone https://github.com/yourusername/aquacare.git
2. Open the project folder and navigate to the index.html file.
3. Host the HTML file on a local server or open it directly in your browser.
4. Ensure that you have set up your Firebase project and linked the Firebase Realtime Database correctly. Replace the values with your Firebase project's credentials. The app uses Firebase to fetch real-time data, so it's crucial that the Firebase Realtime Database is correctly set up.

Project video: https://youtu.be/TZUniJCk1n8?si=w6DDumIXHJZ4bUUB
