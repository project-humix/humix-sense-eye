var nats = require('nats').connect();

nats.publish('humix.sense.eye.command','{"feel":10}');
