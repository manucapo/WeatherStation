let socket = io();
let element = document.getElementById("temperature");
let table = document.getElementById("datatableanchor");
let _temperatureData = 0;
let _humidityData = 0;
let _pressureData = 0;

function generateComponent() {
  let component = `   
     <table>
         <thead>
             <tr>
             <th colspan="3">Data</th>
        </tr>
        <tr>
            <th>Temperature</th>
            <th>Humidity</th>
            <th>Pressure</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <th id="temperature" class="${_temperatureData >25? 'hot': 'cold'}">
                ${_temperatureData} C
            </th>
            <th id="Humidity">
            ${_humidityData} %
            </th>
            <th id="Pressure">
            ${_pressureData} mbar
            </th>
        </tr>
    </tbody>
</table> `;
  let div = document.createElement("div");
  div.innerHTML = component;
  return div;
}

socket.on("temperature", (temperatureData) => {
  _temperatureData= temperatureData;
});

socket.on("humidity", (humidityData) => {
  _humidityData = humidityData;
});

socket.on("pressure", (pressureData) => {
  _pressureData = pressureData;
});

socket.on("update", () => {
  table.innerHTML = '';
  table.appendChild(generateComponent());
});


socket.on("socketDisconnect", (message) => {
  console.log(message);
});

socket.on("socketConnect", (message) => {
  console.log(message);
});
