fs = require('fs');
let csv = require('fast-csv');

// variables -----------------------
var total = 0;
var accounts = 0;
var average = 0;
var max = 0;

// proccessing ---------------------
var stream = fs.createReadStream("6MSnapshotWithBalances.csv");
csv.fromStream(stream, {headers : true})
    .on("data", function(data){
        data.balance = parseFloat(data.balance);
        total += data.balance;
        accounts++;
        if (max < data.balance) max = data.balance;
        console.log(data);
    })
    .on("end", function() {
        average = total / accounts;
        console.log("-----------------------");
        console.log("total:    ", total);
        console.log("average:  ", average);
        console.log("accounts: ", accounts);
        console.log("max:      ", max);
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