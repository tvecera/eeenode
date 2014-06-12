/**
 * Copyright 2013 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

module.exports = function(RED) {
    //var fs = require("fs");
    //var spawn = require('child_process').spawn;
    Tail = require('tail').Tail;
    
    function TailNode2(n) {
        RED.nodes.createNode(this,n);
    
        this.filename = n.filename;
        this.split = n.split;
        var node = this;
    
        var err = "";
	if (node.split) {
	    tail = new Tail(this.filename, "\n\r");
	} else {
	    tail = new Tail(this.filename);
	}
	
	tail.on("line", function(data) {
	    console.log(data);
	});

	tail.on("error", function(error) {
	    console.log('ERROR: ', error);
	});
	
        this.on("close", function() {
	    tail.unwatch();
        });
    }
    
    RED.nodes.registerType("tail2",TailNode2);
}
