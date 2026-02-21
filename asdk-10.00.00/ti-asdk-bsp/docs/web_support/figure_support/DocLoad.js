window.addEventListener('load', function() {
    countFigures();
    getFigCap();
})

function countFigures()
{
  var figNums = document.getElementsByTagName('figure');
  var figCount = 0;
  for (var i=0; i<figNums.length; i++)
  {
    if(figNums[i].id != "")
    {
      figCount++;
      document.getElementById(figNums[i].id).children[1].textContent = "Figure" + " " + 
      String(figCount) + " " + " - " + document.getElementById(figNums[i].id).children[1].textContent;
    }
    
  }
}

function getFigCap() {
    var spans = document.getElementsByTagName('span');
    for (var i=0; i<spans.length; i++)
    {
      if (spans[i].id != "")
      {
        var content = document.getElementById(String(spans[i].id.substring(0, spans[i].id.length-2))).children[1].textContent;
        spans[i].innerText = content.substring(0, content.indexOf(" -"));
      }
    }
}