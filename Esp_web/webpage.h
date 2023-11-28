#ifndef WEBPAGE_H
#define WEBPAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <title>Smart Clock Web Server</title>
  <!-- Bootstrap CSS -->
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
  <style>
    body {
      background-color: #f8f9fa;
    }
    .jumbotron {
      background-color: #343a40;
      color: #ffffff;
    }
    .container {
      margin-top: 20px;
    }
    .info-zone {
      background-color: #e9ecef;
      padding: 15px;
      border-radius: 10px;
      margin-bottom: 20px;
    }
    .update-btn {
      float: right;
    }
    .form-group {
      margin-bottom: 20px;
    }
  </style>
  <script>
    function updateAll() {
      var stockName = document.getElementById('stockInput').value;
      var weatherCountry = document.getElementById('weatherCountry').value;
      var weatherCity = document.getElementById('weatherCity').value;
      var platformNumber = document.getElementById('platformNumber').value;
      var busNumber = document.getElementById('busNumber').value;
      var neopixelRed = document.getElementById('neopixelRed').value;
      var neopixelGreen = document.getElementById('neopixelGreen').value;
      var neopixelBlue = document.getElementById('neopixelBlue').value;

      fetch('/updateAll?name=' + stockName + '&country=' + weatherCountry + '&city=' + weatherCity + '&platform=' + platformNumber + '&bus=' + busNumber + '&red=' + neopixelRed + '&green=' + neopixelGreen + '&blue=' + neopixelBlue)
        .then(response => response.text())
        .then(data => {
          console.log('All information updated:', data);
          fetchData('/stockName', 'stockContent');
          fetchData('/weather', 'weatherContent');
          fetchData('/transportation', 'transportationContent');
          fetchData('/neopixelColor', 'neopixelColorContent');
        });
    }

    function fetchData(endpoint, targetId) {
      fetch(endpoint)
        .then(response => response.text())
        .then(data => {
          document.getElementById(targetId).innerHTML = data;
        });
    }

    // Initial fetch when the page is loaded
    fetchData('/data', 'content');
    fetchData('/stockName', 'stockContent');
    fetchData('/weather', 'weatherContent');
    fetchData('/transportation', 'transportationContent');

    // Update the data every 5 seconds
    setInterval(function() {
      fetchData('/data', 'content');
      fetchData('/stockName', 'stockContent');
      fetchData('/weather', 'weatherContent');
      fetchData('/transportation', 'transportationContent');
    }, 5000);
  </script>
</head>
<body>
  <div class="jumbotron text-center">
    <h1 class="display-4">SMART CLOCK</h1>
    <p class="lead">Access the essential information</p>
  </div>

  <div class="container">
    <!-- Stock Update Zone -->
    <div class="info-zone">
      <h2>Stock</h2>
      <div class="form-group">
        <label for="stockInput">Symbol:</label>
        <input type="text" class="form-control" id="stockInput" placeholder="Ex) AAPL">
      </div>
      <p id="stockContent">Loading...</p>
    </div>

    <!-- Weather Update Zone -->
    <div class="info-zone">
      <h2>Weather</h2>
      <div class="form-group">
        <label for="weatherCountry">Country Code:</label>
        <input type="text" class="form-control" id="weatherCountry" placeholder="Ex) kr">
        <label for="weatherCity">City:</label>
        <input type="text" class="form-control" id="weatherCity" placeholder="Ex) seoul">
      </div>
      <p id="weatherContent">Loading...</p>
    </div>

    <!-- Transportation Update Zone -->
    <div class="info-zone">
      <h2>Transportation</h2>
      <div class="form-group">
        <label for="platformNumber">Platform Number:</label>
        <input type="text" class="form-control" id="platformNumber" placeholder="Ex) 47946">
        <label for="busNumber">Bus Number:</label>
        <input type="text" class="form-control" id="busNumber" placeholder="Ex) 5005">
      </div>
      <p id="transportationContent">Loading...</p>
    </div>

    <!-- LED Update Zone -->
    <div class="info-zone">
      <h2>LED Color</h2>
      <div class="form-group">
        <label for="neopixelRed">Red:</label>
        <input type="number" class="form-control" id="neopixelRed" placeholder="Ex) 0~255" min="0" max="255">
        <label for="neopixelGreen">Green:</label>
        <input type="number" class="form-control" id="neopixelGreen" placeholder="Ex) 0~255" min="0" max="255">
        <label for="neopixelBlue">Blue:</label>
        <input type="number" class="form-control" id="neopixelBlue" placeholder="Ex) 0~255" min="0" max="255">
      </div>
      <p id="neopixelColorContent">Loading...</p>
    </div>

    <!-- Update All Button -->
    <button onclick="updateAll()" class="btn btn-primary update-btn">Update All</button>

    <!-- Display Zones -->
    <div class="form-group">
      <p id="content">Loading...</p>
    </div>
  </div>

  <footer class="text-muted text-center">
    <div class="container">
      <p>© 2023 Smart Clock</p>
      <p id="datetime"></p>
    </div>
  </footer>
</body>
</html>
)rawliteral";

#endif // WEBPAGE_H