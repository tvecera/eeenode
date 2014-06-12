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
    Tail = require('tail').Tail;

    function RedTailNode(n) {
        RED.nodes.createNode(this,n);

        this.filename = n.filename;
        this.split = n.split;
        this.splitstr = n.splitstr;
        var node = this;
        var msg = {topic:node.filename};

        tail = new Tail(node.filename);

	      tail.on("line", function(data) {
          if (node.split && node.splitstr) {
            var strings = data.split(node.splitstr);
            for (s in strings) {
              if (strings[s] != "") {
                msg.payload = strings[s];
                node.log(msg);
                node.send(msg);
              }
            }
          } else {
            msg.payload = data;
            node.log(msg);
            node.send(msg);
          }
	      });

	      tail.on("error", function(error) {
	         node.log(error);
	      });

        this.on("close", function() {
	         tail.unwatch();
        });
    }

    RED.nodes.registerType("redtail",RedTailNode);
}
