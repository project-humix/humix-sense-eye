var nats = require('nats').connect();
var log = require('logule').init(module, 'eye');
var SerialPort = require('serialport').SerialPort;
var portName = '/dev/ttyACM0';

var sp = new SerialPort(portName, {
//   baudRate: 115200,
   baudRate: 9600,
   dataBits: 8,
   parity: 'none',
   stopBits: 1,
   flowControl: false
});




sp.open(function(error){


  if ( error ) {
    console.log('failed to open: '+error);
  } else {
      log.info('Eye control is ready');
      sp.write('0\n');
      // sleep by default
      sp.write('SLE');

  }});


// health check
nats.subscribe('humix.sense.eye.status.ping', function(request,replyto){

    nats.publish(replyto,'humix.sense.eye.status.pong');
})


nats.subscribe('humix.sense.eye.command', function(input){

   log.debug('received eye control message, msg:'+input);

    var msg = JSON.parse(input);

    if( msg.action ){

        if(msg.action === 'wakeup'){
            log.info("Waking Up");
            sp.write('WKE');
        }else if(msg.action === 'sleep'){

            log.info("Sleeping");
            sp.write('SLE');
        }
        
    }else if (msg.feel && msg.feel === 'excited'){

        log.info("Getting exciting !")
        sp.write('XXE');
        
    }else if( msg.feel && msg.feel >= -100 && msg.feel <= 100 ){

        log.info("Getting feel command");
        var level = parseInt(msg.feel);
        var prefix,command;
        if(level >= 0)
            prefix = "FP";
        else{
            prefix = "FN";
        }


        command = prefix + Math.floor((Math.abs(level)/100)*255);
        console.log('command:'+command)
        sp.write(command+'E');
	}else if( msg.heartrate ){

        log.info("received heart rate");
        var command = "HR"+msg.heartrate;
        log.info("command :"+command);
        sp.write(command+'E');
    } 
    
});



