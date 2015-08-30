var lnklib = require('../lib/index.js');

var Library = lnklib.Library;

var kernel32 = new Library('kernel32.dll');

var GetStdHandle = kernel32.linkFunction('GetStdHandle', lnklib.Types.pointer, [
	lnklib.Types.ulong
]);

var handle = GetStdHandle(-11);