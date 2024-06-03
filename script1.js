var websocket, websocket82, websocket83, websocket84, websocket85;



function initWebSocket() 
{
  websocket = new WebSocket("ws://" + window.location.hostname + ":81");
  websocket.onopen = function(event) {
    console.log("Conectado al WebSocket");
  };
  websocket.onclose = function(event) {
    console.log("Desconectado del WebSocket");
  };
  websocket.onmessage = function(event) {
    console.log("Mensaje recibido: " + event.data);
    var data = event.data;
    document.getElementById('response').textContent = data;
    setTimeout(()=>{
        document.getElementById('response').textContent = '';
    }, 2500);
  };
  websocket.onerror = function(event) {
    console.error("Error en WebSocket: ", event);
  };




  websocket82 = new WebSocket("ws://" + window.location.hostname + ":82");
  websocket82.onopen = function(event) {
    console.log("Conectado al WebSocket del puerto 82");
  };

  websocket82.onclose = function(event) {
    console.log("Desconectado del WebSocket del puerto 82");
  };

  websocket82.onmessage = function(event) {
    // Manejar los mensajes recibidos en el puerto 82
    console.log("Mensaje recibido en el puerto 82: " + event.data);
    // Aquí puedes procesar los datos recibidos del puerto 82
    var data2 = event.data;
    document.getElementById('humedad_lectura').textContent = data2;
    setTimeout(()=>{
        document.getElementById('humedad_lectura').textContent = '';
    }, 2500);
  };

  websocket82.onerror = function(event) {
    console.error("Error en WebSocket del puerto 82: ", event);
  };



  websocket83 = new WebSocket("ws://" + window.location.hostname + ":83");
  websocket83.onopen = function(event) {
    console.log("Conectado al WebSocket del puerto 83");
  };

  websocket83.onclose = function(event) {
    console.log("Desconectado del WebSocket del puerto 83");
  };

  websocket83.onmessage = function(event) {
    // Manejar los mensajes recibidos en el puerto 82
    console.log("Mensaje recibido en el puerto 83: " + event.data);
    // Aquí puedes procesar los datos recibidos del puerto 82
    var data3 = event.data;
    document.getElementById('result').textContent = data3;
    setTimeout(()=>{
        document.getElementById('result').textContent = '';
    }, 15000);
  };

  websocket83.onerror = function(event) {
    console.error("Error en WebSocket del puerto 83: ", event);
  };



  websocket84 = new WebSocket("ws://" + window.location.hostname + ":84");
  websocket84.onopen = function(event) {
    console.log("Conectado al WebSocket del puerto 84");
  };

  websocket84.onclose = function(event) {
    console.log("Desconectado del WebSocket del puerto 84");
  };

  websocket84.onmessage = function(event) {
    // Manejar los mensajes recibidos en el puerto 84
    console.log("Mensaje recibido en el puerto 84: " + event.data);
    // Aquí puedes procesar los datos recibidos del puerto 84
    var data4 = event.data;
    console.log("Cambio estado de luz");
  };

  websocket84.onerror = function(event) {
    console.error("Error en WebSocket del puerto 84: ", event);
  };






  websocket85 = new WebSocket("ws://" + window.location.hostname + ":85");
  websocket85.onopen = function(event) {
    console.log("Conectado al WebSocket del puerto 85");
  };

  websocket85.onclose = function(event) {
    console.log("Desconectado del WebSocket del puerto 85");
  };

  websocket85.onmessage = function(event) {
    // Manejar los mensajes recibidos en el puerto 85
    console.log("Mensaje recibido en el puerto 85: " + event.data);
    // Aquí puedes procesar los datos recibidos del puerto 85
    var data5 = event.data;
    document.getElementById('humedad_suelo_lectura').textContent = data5;
    setTimeout(()=>{
        document.getElementById('humedad_suelo_lectura').textContent = '';
    }, 5000);
  };

  websocket85.onerror = function(event) {
    console.error("Error en WebSocket del puerto 85: ", event);
  };
}

function SolicitarTemperatura() {
  if (websocket.readyState === WebSocket.OPEN) {
    websocket.send("getTemperature");
  } else {
    alert("WebSocket no está conectado.");
  }
}

function SolicitarHumedad() {
  if (websocket82.readyState === WebSocket.OPEN) {
    websocket82.send("getHumedad");
  } else {
    alert("WebSocket no está conectado.");
  }
}

function enviarTemperatura() {
    if (websocket83.readyState === WebSocket.OPEN) {
        const temp = document.getElementById('temperature').value;
      websocket83.send(temp);
    } else {
      alert("WebSocket no está conectado.");
    }
  }


function AccionLuz() {
    if (websocket84.readyState === WebSocket.OPEN) {
      websocket84.send("Luz");
    } else {
      alert("WebSocket no está conectado.");
    }
  }

function HumedadDeSuelo() {
    if (websocket85.readyState === WebSocket.OPEN) {
      websocket85.send("HumedadSuelo");
    } else {
      alert("WebSocket no está conectado.");
    }
  }


window.onload = initWebSocket;
