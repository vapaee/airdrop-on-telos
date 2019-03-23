// This airdrop is based on the first Telos orignal snapshot and the new claim action developped for this goal.
// This script reeads the snapshot from the local csv_file and performs a call to the claim action of the smart contract.
// To run a simulation set simulate = true. This will not execute any cleos command.

var fs = require('fs');
var sys = require('sys')
var exec = require('child_process').exec;
let csv = require('fast-csv');

// Constants -----------------------
var simulate = true; // if true, the script will not execute the actual commands to the blockchain
var csv_file = "6MSnapshotWithBalances.csv";
var contract_name = "vapaeetokens";
var ram_payer = "vapaeetokens";
var local = "cleos";
var telostest = "cleos --url https://testnet.telos.caleos.io";
var telosmain = "cleos --url https://telos.eos.barcelona";
var milliseconds = 0;
var delay = 0;

// --- which blockchain will you use ?
var cleos = telosmain;

// --- cropping the target accounts (modify them to adjust your needs)
var min = 0; // receiver must have at least min TLOS to be added in this airdrop
var max = 0; // receiver must have less than max TLOS to be added in this airdrop (0 means no effect)
var cap = 0; // receiver airdrop will be capped at cap


// -- auxiliar variables (don't modify)
var total = 0;
var accounts = 0;
var capped = 0;
var average = 0;
var command;

// -------------
// the following example will perform an aidrop where only users between [100] and (1000)
// will receive an amount of tokens equal to the amount of TLOS but capped at 40K.
// min = 100;
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
var stream = fs.createReadStream(csv_file);
csv.fromStream(stream, {headers : true})
    .on("data", function(data) {
        data.balance = parseFloat(data.balance);
        if (cap > 0) if (data.balance > cap) {
            data.balance = cap;
            capped++;
        }
        if (data.balance < min) return;
        if (0 < max) if (data.balance >= max) return;

        total += data.balance;
        accounts++;

        // --------------------
        account = data.account;
        balance = Math.floor(data.balance * 10000);
        // push to local net ----
        command = cleos + " push action " + contract_name + " claim '[\""+account+"\",\"CNT\",\"" + ram_payer + "\"]' -p " + ram_payer + "@active";
        commands.push(command);
    })
    .on("end", function() {
        average = total / accounts;
        console.log("-----------------------");
        console.log("cap:      ", cap);
        console.log("min:      ", min);
        console.log("max:      ", max);
        console.log("-----------------------");
        console.log("total:    ", total);
        console.log("average:  ", average);
        console.log("accounts: ", accounts);
        console.log("capped:   ", capped);
        if (!simulate) process_snapshot();
    });

