var lnklib = require('../build/Release/lnklib.node');

function LibraryModule()
{
	this.id = undefined;
	this.filepath = undefined;
	this.functions = [];
	
	if(arguments.length > 0) {
		this.load.apply(this, arguments);
	}
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
		
		return lnklib.unload(this.id);
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

LibraryModule.prototype.linkFunction = function(identifier) {
	var id = this._getLinkId(identifier);
	if(id === 0) {
		id = lnklib.getFunction(this.id, identifier);
	
		this.functions.push({
			id: id,
			identifier: identifier
		});
	}
	
	return function() {
		console.log('TODO: Call function');
	}
};

module.exports = LibraryModule;