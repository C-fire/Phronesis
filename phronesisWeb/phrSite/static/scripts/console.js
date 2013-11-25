$.ajaxSetup ({
    // Disable caching of AJAX responses
    cache: false
});

var colorCode = new Array();
colorCode['NEW_MSG'] = 'green';
colorCode['NEW_LOG'] = 'black';
colorCode['NEW_QUESTION'] = 'blue';
colorCode['NEW_YN_QUESTION'] = 'blue';
colorCode['NEW_PB_QUESTION'] = 'blue';
colorCode['CURRENT_PB'] = 'orange';
colorCode['ERROR_TEXT'] = 'red';


var nameCode = new Array();
nameCode['NEW_MSG'] = 'Message';
nameCode['NEW_LOG'] = '';
nameCode['NEW_QUESTION'] = 'Question';
nameCode['NEW_YN_QUESTION'] = 'Question';
nameCode['NEW_PB_QUESTION'] = 'Question';
nameCode['CURRENT_PB'] = 'Current problems';
nameCode['ERROR_TEXT'] = 'ERROR';

window.setInterval(refreshConsole, 5000);

function refreshConsole() {  
//    alert('Refresh');
//    $("#console").load("http://127.0.0.1:8000/static/scripts/demo_test.txt");
    $("#console").html("");
$.ajax({
    type: "GET",
    //url: "http://devphronesis/static/scripts/phronesisXmlOutput.xml",
    url: "http://127.0.0.1:8000/static/scripts/demo_test.xml",
    dataType: "xml",
    success: function(xml){
    $(xml).find('entry').each(function(){
      var sType = $(this).find('type').text();
      var sText = $(this).find('text').text();
      var sDate = $(this).find('date').text();
      var sTime = $(this).find('time').text();
      var color = 'black';
      var type = sType;
      if (sType in colorCode){
          color = colorCode[sType];
      }
      if (sType in nameCode){
          type = nameCode[sType];
      }
      $("#console").append( "<span style=\"color:" + color + "\">" + sDate + " " + sTime + " : " + type + " : " + sText + "</span>\n");
    });
  },
  error: function() {
    alert("An error occurred while processing XML file. (http://127.0.0.1:8000/static/scripts/demo_test.xml)");
  }
  });
    //$("#console").load("/scripts/demo_test.txt");
}

