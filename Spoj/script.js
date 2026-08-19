/*
*   function to set the filter over image
*/

function setFacebookFilter(imgwidth,imgHeight,canvasWidth,canvasHeight,
                           fltrSrc,imgSrc){
    var canvasElement = document.getElementById("myCanvas");
    canvasElement.width = canvasWidth;
    canvasElement.height = canvasHeight;
    var ctx = canvasElement.getContext("2d");
    var image = new Image();
    var filter = new Image();
    image.setAttribute('crossOrigin', 'anonymous');
    filter.setAttribute('crossOrigin', 'anonymous');
    image.width = imgwidth;
    image.height = imgHeight;
    filter.src = fltrSrc;
    filter.onload=function(){
        //image.src = "https://graph.facebook.com/<?php echo $_SESSION['id']; ?>/picture?width=600&height=600";     
        //image.src = "https://graph.facebook.com/100006054881490/picture?width=600&height=600"; 
        image.src = imgSrc;
        image.onload = function() {
            ctx.drawImage(image, 0, 0, image.width, image.height);
            ctx.drawImage(filter, 0, 0, image.width, image.height);
            $("#baap").css("display","block");
        }
    }
}

/*
*   function to download the canvas  
*/

function downloadFilteredImage(link, canvasId, filename) {
link.href = document.getElementById(canvasId).toDataURL();
link.download = filename;
}

/** 
* The event handler for the link's onclick event. We give THIS as a
* parameter (=the link element), ID of the canvas and a filename.
*/
document.getElementById('download').addEventListener('click', function() {
downloadFilteredImage(this, 'myCanvas', 'incand.png');
}, false);

