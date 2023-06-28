var bodyParser = require('body-parser');
var urlencodedParser = bodyParser.urlencoded({ extended: false });

module.exports = function(app, mqtt, mongoose){
  var client  = mqtt.connect('mqtt://192.168.0.109:1883');
  var rainSensor = mongoose.model('Rain', new mongoose.Schema({data: Number}));
  var humidSensor = mongoose.model('Humidity value', new mongoose.Schema({name: String, data: {type: Schema.type.ObjectId, ref: 'sensorData'}}));
  var sensorData = mongoose.model('Reading', new mongoose.Schema({data: Number}));
  var stateModel = mongoose.model('State', new mongoose.Schema({value: String}));
  mongoose.Promise = global.Promise;
  var option = {
    useMongoClient: true,
    keepAlive: 10000,
    connectTimeoutMS: 30000,
    socketTimeoutMS : 30000,
    autoReconnect: true,
    poolSize: 10,
    ha: true
  };

  //Subscribe topic when connected to the MQTT broker
  client.on('connect', function () {
    client.subscribe('data/sensor');
    client.subscribe('data/confirm');
  });
  //Handle message arrived
  client.on('message', function (topic, message) {
    //console.log(message.toString());
      if(topic.toString() === 'data/sensor'){
        //Connect to the db
        var db = mongoose.connect('mongodb://localhost/mydb', option, function(err){
		if(message.toString() === 'rain'){
		sensorModel.findOneAndUpdate({value: /(rain).*/},
		{value: var
		)}
          /*if (err) return err;
          //if number of collection < 1000 then save its to the db
          sensorModel.count({}, function(err, result){
            if(result < 1000)
              sensorData = new sensorModel({data: parseFloat(message)}).save(function(err){
                if (err) return err;
                db.close(function(){
                  console.log('sensor connection close');
                });
              });
            //if number of collection > 1000 then delete it from the db
            else{
              sensor.deleteMany({}, function(err){
                if (err) return err;
                db.close(function(){
                  console.log('sensorModel connection close');
                });
              });
            }
          });*/
        });

      }
      else if (topic.toString() === 'data/confirm') {
        //Update the stateModel of digital pin to db
        var db = mongoose.connect('mongodb://localhost/mydb', option, function(err){
          if(message.toString() === 'autoon' || message.toString() === 'autooff') {
          stateModel.findOneAndUpdate({value: /(auto).*/},
            {value: message.toString()},
            {upsert: true},
            function(err, doc){
              db.close(function(){
                console.log('confirm close');
              });
            });
          }
          else if(message.toString() === 'pump1on' || message.toString() === 'pump1off') {
            stateModel.findOneAndUpdate({value: /(pump1).*/},
              {value: message.toString()},
              {upsert: true},
              function(err, doc){
                db.close(function(){
                  console.log('confirm close');
                });
              });
          }
          else if(message.toString() === 'pump2on' || message.toString() === 'pump2off') {
            stateModel.findOneAndUpdate({value: /(pump2).*/},
              {value: message.toString()},
              {upsert: true},
              function(err, doc){
                db.close(function(){
                  console.log('confirm close');
                });
              });
           }
		   else if(message.toString() === 'pump3on' || message.toString() === 'pump3off') {
            stateModel.findOneAndUpdate({value: /(pump3).*/},
              {value: message.toString()},
              {upsert: true},
              function(err, doc){
                db.close(function(){
                  console.log('confirm close');
                });
              });
           }
		   else if(message.toString() === 'pumpall' || message.toString() === 'stoppump') {
            stateModel.findOneAndUpdate({value: /(all pump).*/},
              {value: message.toString()},
              {upsert: true},
              function(err, doc){
                db.close(function(){
                  console.log('confirm close');
                });
              });
           }
		   else if(message.toString() === 'upout' || message.toString() === 'upin') {
            stateModel.findOneAndUpdate({value: /(upper tray).*/},
              {value: message.toString()},
              {upsert: true},
              function(err, doc){
                db.close(function(){
                  console.log('confirm close');
                });
              });
           }
		   else if(message.toString() === 'lowout' || message.toString() === 'lowin') {
            stateModel.findOneAndUpdate({value: /(lower tray).*/},
              {value: message.toString()},
              {upsert: true},
              function(err, doc){
                db.close(function(){
                  console.log('confirm close');
                });
              });
           }
        });
      }
    });


  //Handle get request
  app.get('/', function(req, res){
    var db = mongoose.connect('mongodb://localhost/mydb', option, function(err){
      if (err) return err;
      stateModelModel.find({}, function(err, result){
        if (err) return err;
        res.render('index', {data: result});
        db.close(function(){
          console.log('get close');
        });
      });
    });
  });
  //Handle post request
  app.post('/', urlencodedParser, function(req, res){
    var db = mongoose.connect('mongodb://localhost/mydb', option,function(err){
      if (err) return err;
      //Send the sensorModel value back to the client
      sensorModelModel.find({}, function(err, result){
        if (err) return err;
        var time = 0;
        var arr = [];
        for(var i=0; i<result.length;i++){
          arr.push([time, result[i].data]);
          time += 2;
        }
        res.send(arr);
        db.close(function(){
          console.log('post connection close');
        });
      });
    });
  });
};