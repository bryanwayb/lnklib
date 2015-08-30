var lnklib = require('../build/Release/lnklib.node');

function LibraryModule()
{
	this._init();
	
	if(arguments.length > 0) {
		this.load.apply(this, arguments);
	}
}

LibraryModule.prototype._init = function() {
	this.id = undefined;
	this.filepath = undefined;
	this.functions = [];
}

LibraryModule.prototype.load = function(filepath) {
	if(this.id) {
		this.close();
	}
	this.id = lnklib.load(filepath);
	this.filepath = filepath;
};

LibraryModule.prototype.close = function() {
	if(this.id) {
		for(var i in this.functions) {
			lnklib.clearFunction(this.functions[i].id);
		}
		
		var result = lnklib.unload(this.id);
		this._init();
		return result;
	}
	return false;
};

LibraryModule.prototype._getLinkId = function(identifier) {
	for(var i in this.functions) {
		if(this.functions[i].identifier == identifier) {
			return this.functions[i].id;
		}
	}
	return 0;
};

LibraryModule.prototype.linkFunction = function(identifier, returnType, paramMap) {
	if(arguments.length < 3) {
		throw new Error('Not enough arguments. An identifier, return type, and parameter map are required');
	}
	
	var id = this._getLinkId(identifier);
	if(id === 0) {
		id = lnklib.getFunction(this.id, identifier);
	
		this.functions.push({
			id: id,
			identifier: identifier
		});
	}
	
	return function() {
		console.log('Executing function...');
		lnklib.execute(id, returnType, paramMap, arguments);
		console.log('Done');
	}
};

module.exports = {
	Library: LibraryModule,
	Types: {
		'void': 		0x0,
		'pointer':		0x1,
		'char':			0x2,
		'uchar':		0x3,
		'short':		0x4,
		'ushort':		0x5,
		'int':			0x6,
		'uint':			0x7,
		'long':			0x8,
		'ulong':		0x9,
		'longlong':		0xA,
		'ulonglong':	0xB,
		'array': function(length, arrayType) {
			if(!length) {
				throw new Error('Array length must be defined');
			}
			else if(!arrayType) {
				throw new Error('Array type must be defined');
			}
			return {
				_type: 0xC,
				_length: length,
				_arrayType: typeof arrayType === 'string' ? module.exports.Types[arrayType] : arrayType
			};
		},
		'darray': function(arg, arrayType) { // Same thing as array, except when the length of the array will be detirmined by an output parameter (starting from index arg=0) or a return value (-1)
			if(!arg) {
				throw new Error('Array length source parameter must be defined');
			}
			else if(!arrayType) {
				throw new Error('Array type must be defined');
			}
			return {
				_type: 0xD,
				_length: arg,
				_arrayType: typeof arrayType === 'string' ? module.exports.Types[arrayType] : arrayType
			};
		},
		'string': function() {
			return {
				_type: 0xE,
				_length: arguments.length > 0 && typeof arguments[0] === 'number' ? arguments[0] : -1
			}
		},
		'struct': function(definition) {
			return {
				_type: 0xF,
				_definition: definition
			}
		}
	}
};