import { SerialPortMock, SerialPort, ReadlineParser, ByteLengthParser } from 'serialport';
import express from 'express';
import { Server }  from 'socket.io';
import  http  from 'http';



const port = 3000;
const app = express();
const root = "XXXXXXXXXXXXXXXXXXX"

let _temperatureData = '';
let _humidityData = '';
let _pressureData = '';

let server = http.createServer(app);
const io = new Server(server);


const getPorts = async function(){
    var ports = await SerialPort.list();
    console.log(ports);
}

const processData = function(data){
    var buffer = Buffer.from(data);
    let sensorData = buffer.toString('utf8');
    let id =  sensorData.substring(0,1);

   switch (id) {
    case '1':
        _temperatureData = Number(sensorData.substring(1));
        console.log(`Temperature: ${_temperatureData}` );
        break;
    case '2':
        _humidityData = Number(sensorData.substring(1));
        console.log(`Humidity: ${_humidityData}` );
        break;
    case '3':
        _pressureData = Number(sensorData.substring(1));
        console.log(`Pressure: ${_pressureData}` );
        break;
    default:
        break;
   }
}

const serialport = new SerialPort({path: 'COM4', baudRate: 9600});
const parser = new ByteLengthParser({length:5});

serialport.pipe(parser);
parser.on('data', processData);


app.get('/', (req, res) =>{

    res.sendFile('/index.html', {root: root});
});

app.use('/static',express.static(root + '/static'))

io.on('connection', (socket) =>{

    socket.broadcast.emit('socketConnect', 'User connected');

    let interval = setInterval(() => {
        console.log('SENDING MESSAGE');
        socket.emit('temperature',_temperatureData);
        socket.emit('humidity',_humidityData);
        socket.emit('pressure',_pressureData)
        socket.emit('update');
      }, 4000);

      socket.on('disconnect', () =>{
        clearInterval(interval)
        console.log('DISCONNECTED');
    });

    socket.on('disconnect', () => {
        io.emit('socketDisconnect','User disconnected');
    });
});



server.listen(port,'[::]', () => console.log(`Listening on port ${port}`));




