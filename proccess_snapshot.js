fs = require('fs');
let csv = require('fast-csv');

// variables -----------------------
var total = 0;
var accounts = 0;
var capped = 0;
var average = 0;
var max = 0;
var min = 0;
var cap = 0;

// max = 0;
min = 1;
cap = 40000;
// proccessing ---------------------
var stream = fs.createReadStream("6MSnapshotWithBalances.csv");
csv.fromStream(stream, {headers : true})
    .on("data", function(data) {
        data.balance = parseFloat(data.balance);
        if (cap > 0) if (data.balance > cap) {
            data.balance = cap;
            capped++;
        }
        if (data.balance < min) return;
        // if (0 < max) if (data.balance > max) return;

        total += data.balance;
        accounts++;
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
    });


/* ---- min = 1 -----
total:  49619147.807394534
accounts:  21778
average:  2278.4070074108977    
*/

/* ---- min = 0 -----
total:  49619386.01709602
accounts:  26418
average:  1878.2415783593012   
*/