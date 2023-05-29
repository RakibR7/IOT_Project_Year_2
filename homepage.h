const char* homePagePart1 = (R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Home Automation Website</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        font-size: 16px;
        line-height: 1.5;
        margin: 0;
        padding: 0;
        //heropage
        color: white;
        //background-color: #000000; 
      }

      .flex-container {
        display: flex;
        flex-direction: column;
        align-items: center;
        padding: 16px;
      }

      h1 {
        font-size: 40px;
        font-weight: bold;
        margin: 16px;
        //color: white;
      }
      //paragraphs
      p {
        font-size: 25px;
        font-family: Arial;
        margin: 16px;
      }


      table {
        width: 80%;
        max-width: 600px;
        border-collapse: collapse;
        margin: 16px;
        background-color: #1A594F;
        color: #fff;
        border-radius: 6px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      }

      //table header 
      th {
        padding: 0.5rem;
        text-align: center;
        border-bottom: 2px solid #84DFD0;
        font-size: 20px;
        background-color: #3FD9BF;
      }

      //table rows 
      td {
        padding: 8px;
        text-align: center;
        border-bottom: 1px solid #355953;
        font-size: 18px;
      }

      // first column of the table
      td:first-child {
        font-weight: bold;
      }



      // Button styles
      .button-container {
        display: flex;
        justify-content: center;
      }

       button {
        background-color: #3FD9BF;
        border: none;
        color: white;
        padding: 12px 24px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        margin: 10px 8px; /* Adjusted margin to separate the buttons */
        cursor: pointer;
        border-radius: 4px;
        transition-duration: 0.4s;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      }

        button:hover {
        background-color: #84DFD0;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
      }

      button:disabled {
        background-color: #ccc;
        cursor: not-allowed
      }          
</style>


<script>
      //javascript all about updating data
      function onConnect() {
        GetFromServer
        updateModeUI();
      }
      //document methods used for retrieving data and updating realtime
      //gets a reference to the button, id is used to modify it
      function updateSensorData(temperature, humidity, light, servo) {
        document.getElementById("temp").innerText = temperature;
        document.getElementById("humidity").innerText = humidity;
        document.getElementById("light").innerText = light;
        document.getElementById("servo").innerText = servo;
      }
      
      //uses promise based to return a result
      async function GetFromServer() {
        try {
          const responseDHT = await fetch("/getDHT");
          const responseLDR = await fetch("/getLDR");
          const responseServo = await fetch("/getServo");
          const dhtData = await responseDHT.text();
          const ldrData = await responseLDR.text();
          const servoData = await responseServo.text();
          const [temperature, humidity] = dhtData.split(" ");
          updateSensorData(temperature, humidity, ldrData, servoData);
        } catch (error) {
          console.error("Error fetching data:", error);
        }
      }

      //update element on page
      async function LightMode() {
        try {
          const response = await fetch("/LightMode");
          const newState = await response.text();
          console.log("New light state:", newState);
          document.getElementById("lightStatus").innerText = newState;
        } catch (error) {
          console.error("Error toggling light:", error);
        }
      }
      //await is just for wating until promise is returned
      //pauses everything until data is in JSON
      async function ServoMode() {
        try {
          const response = await fetch("/ServoMode");
          const newState = await response.text();
          console.log("New servo state:", newState);
          document.getElementById("servoStatus").innerText = newState;
        } catch (error) {
          console.error("Error toggling servo:", error);
        }
      } 

      async function moveServo() {
        try {
          await fetch(`/servo`);
          console.log("Servo moved");
        } catch (error) {
          console.error("Error moving servo:", error);
        }
      }

      var controlMode = 0; // Start with manual mode by default
      var servoControlMode = 0; // Start with manual mode by default (1: Manual, 0: Auto)

      function switchMode() {
         fetch('/switchMode')
        .then(function (response) {
          return response.text();
      })
      .then(function (text) {
      if (text == "auto") {
        controlMode = 0;
      } else {
        controlMode = 1;
      }
      updateLightModeUI();
      })
      .catch(function (error) {
        console.error('Error switching mode:', error);
      });
      }

      function updateLightModeUI() {
          if (controlMode == 1) { // Manual mode
            document.getElementById("LightModeBtn").disabled = false;
            document.getElementById("switchModeBtn").innerHTML = "Switch to Auto"; // add this line
          } else { // Auto mode
            document.getElementById("LightModeBtn").disabled = true;
            document.getElementById("switchModeBtn").innerHTML = "Switch to Manual"; // add this line
          }
      }




      function switchServoMode() {
      fetch('/switchServoMode')
        .then(function (response) {
         return response.text();
      })
      .then(function (text) {
        if (text == "auto") {
          servoControlMode = 0;
        } else {
          servoControlMode = 1;
        }
        updateServoModeUI();
      })
      .catch(function (error) {
        console.error('Error switching servo mode:', error);
      });
     }


    function updateServoModeUI() {
      if (servoControlMode == 1) { // Manual mode
        document.getElementById("switchServoModeBtn").innerHTML = "Switch Servo to Auto";
        document.getElementById("ServoModeBtn").disabled = false;
      } 
      else // Auto mode
      { 
        document.getElementById("switchServoModeBtn").innerHTML = "Switch Servo to Manual";
        document.getElementById("ServoModeBtn").disabled = true;
      }
    }
    
    function updateModeUI() {
      updateLightModeUI();
      updateServoModeUI();
    }


    //event done loading
    window.onload = onConnect;
    setInterval(GetFromServer, 2000);
</script>


</head>
  <body>
    <div class="flex-container">
      <h1>Home Automation Website</h1>
      <p>Sensor readings:</p>
      <table>
        <thead>
          <tr>
            <th>Sensor</th>
            <th>Value</th>
            <th>Unit</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>Temperature</td>
            <td>
              <span id="temp">Temp_PlaceHolder</span>
            </td>
            <td>&deg;C</td>
          </tr>
          <tr>
            <td>Humidity</td>
          <td>
            <span id="humidity">Humidity_PlaceHolder</span>
          </td>
          <td>%</td>
        </tr>
        <tr>
          <td>Light</td>
          <td>
            <span id="light">Light_PlacHolder</span>
          </td>
          <td>lux</td>
        </tr>
        <tr>
          <td>Servo</td>
          <td>
            <span id="servo">Servo_PlaceHolder</span>
          </td>
          <td>degrees</td>
        </tr>
      </tbody>
      </table>   
      <div class="button-container">
        <div>
          <button id="LightModeBtn" onclick="LightMode()" disabled>Toggle Light</button>
          <button id="switchModeBtn" onclick="switchMode()">Switch to Manual</button>
        </div>
        <div>
          <button id="ServoModeBtn" onclick="moveServo()">Move Servo</button>
          <button id="switchServoModeBtn" onclick="switchServoMode()">Switch Servo to Manual</button>
        </div>
      </div>
    </div>
  </body>
</html>
)=====");

