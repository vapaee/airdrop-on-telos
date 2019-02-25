var fs = require('fs');
var sys = require('sys')
var exec = require('child_process').exec;
let csv = require('fast-csv');

// variables -----------------------
var local = "cleos";
var telostest = "cleos --url https://testnet.telos.caleos.io";
var telosmain = "cleos --url https://telos.eos.barcelona";
var cleos = telosmain;
var milliseconds = 0;
var delay = 0;
var command;
// ---
var total = 0;
var accounts = 0;
var capped = 0;
var average = 0;
var min = 0;
var max = 999999999;
var cap = 0;

// -------------
min = 100;
max = 1000;
// cap = 40000;

var commands = [];

// proccessing ---------------------
function process_snapshot() {
    var command = commands.pop();
    console.log(commands.length, command);
    
    dir = exec(command, function(err, stdout, stderr) {
        if (err) {
            console.error("Error: ", err);
            console.debug("reExe: ", command);
            delay = 1000;
        }
    });
      
    dir.on('exit', function (code) {
        // console.log("B-exit code: ", code);
        if (commands.length > 0) {
            // if (delay > 0) return;
            setTimeout(process_snapshot, delay);
            delay = milliseconds;                
        }
    });
}
var stream = fs.createReadStream("6MSnapshotWithBalances.csv");
csv.fromStream(stream, {headers : true})
    .on("data", function(data) {
        data.balance = parseFloat(data.balance);
        if (cap > 0) if (data.balance > cap) {
            data.balance = cap;
            capped++;
        }
        if (data.balance < min) return;
        if (data.balance >= max) return;
        // if (accounts > 3) return;
        // if (0 < max) if (data.balance > max) return;

        total += data.balance;
        accounts++;

        // --------------------
        account = data.account;
        balance = Math.floor(data.balance * 10000);
        // push to local net ----
        command = cleos + " push action vapaeetokens claim '[\""+account+"\",\"CNT\",\"vapaeetokens\"]' -p vapaeetokens@active";
        commands.push(command);
    })
    .on("end", function() {
        average = total / accounts;
        console.log("-----------------------");
        console.log("cap:      ", cap);
        console.log("min:      ", min);
        console.log("-----------------------");
        console.log("total:    ", total);
        console.log("average:  ", average);
        console.log("accounts: ", accounts);
        console.log("capped:   ", capped);
        process_snapshot();
    });

