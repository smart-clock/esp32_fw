// webpage.h

#ifndef WEBPAGE_H
#define WEBPAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <title>2023 SMART CLOCK Web Server</title>
  <!-- Bootstrap CSS -->
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
  <script>
    function fetchData() {
      fetch('/data')
        .then(response => response.text())
        .then(data => {
          document.getElementById('content').innerHTML = data;
        });
    }

    // Initial fetch when the page is loaded
    fetchData();

    // Update the data every 1 second
    setInterval(fetchData, 1000);
  </script>
</head>
<body class="bg-light">
  <div class="jumbotron text-center">
    <h1 class="display-4">SMART CLOCK</h1>
    <p class="lead">"Access the essential information"</p>
  </div>

  <div class="container">
    <p>Value: <span id="content">Loading...</span></p>
  </div>

  <footer class="text-muted text-center">
    <div class="container">
      <p>&copy; 2023 Smart Clock</p>
      <p id="datetime"></p>
    </div>
  </footer>

  <!-- Bootstrap JS and dependencies -->
  <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.3/dist/umd/popper.min.js"></script>
  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
</body>
</html>
)rawliteral";

#endif // WEBPAGE_H