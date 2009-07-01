document.observe("dom:loaded", function() {
	dp.SyntaxHighlighter.HighlightAll('code');
	var g = new k.Growler({location:"tr"});
	$("e1").observe("click", function(e){
		g.growl("Simple notice");
	});
	$("e2").observe("click", function(e){
		g.growl("...with header", {header: "Growler Notice"});
	});
	$("e3").observe("click", function(e){
		g.growl("Long lasting notice (20s)", {life: 20});
	});
	$("e4").observe("click", function(e){
		g.growl("Sticky notice", {sticky: true});
	});
	$("e5a").observe("click", function(e){
		g.growl("Candy is good", {header: "Candybar Theme", className: "candybar", sticky: true});
	});
	$("e5b").observe("click", function(e){
		g.growl("Visit <a href='http://kproto.googlecode.com' target='_blank'>kProto</a> for more Prototype classes.", {className: "plain"});
	});
	$("e5c").observe("click", function(e){
		g.growl("The funnest iPod ever. Millions of songs. Thousands of movies. Hundreds of games. <a target='_blank' href='http://www.apple.com/ipodtouch/whatsnew.html'>Learn more</a>", {header: "iPod Touch", className: "macosx", sticky: true});
	});
	$("e5d").observe("click", function(e){
		g.growl("This is a test to see how well the theme handles text that is long. It should stretch height-wise.", {header: "At Work Theme", className: "atwork"});
	});
	$("e6").observe("click", function(e){
		g.info("Something good happended", {life: 10});
	});
	$("e7").observe("click", function(e){
		g.warn("Take heed", {life: 10});
	});
	$("e8").observe("click", function(e){
		g.error("Something bad happened", {life: 10});
	});
	$("e9").observe("click", function(e){
		g.growl("Notice w/Events", {created: function(){
			$("noticeevents").innerHTML += "<div>Notice created...</div>";
		}, destroyed: function(){
			$("noticeevents").innerHTML += "<div>...Notice destroyed</div>";
		}});
	});
});
