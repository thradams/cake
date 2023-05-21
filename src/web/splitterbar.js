const SplitterBar = function(container, leftContent, rightContent) {
    // We want two divs that we're dividing
    const leftSide = document.createElement('div');
    const rightSide = document.createElement('div');
    const splitter = document.createElement('div');

    leftSide.classList.add('leftSide');
    rightSide.classList.add('rightSide');
    splitter.classList.add('splitter');

    if (leftContent !== null) {
        leftSide.appendChild(leftContent);
    }

    if (rightContent !== null) {
        rightSide.appendChild(rightContent);
    }

    
    container.appendChild(splitter);
    
    splitter.style.width = '10px';
    splitter.style.left = '50%';
    splitter.style.transform = 'translateX(-50%)';
    
    splitter.style.background = 'gray';
    splitter.style.cursor = 'col-resize';
    

    leftSide.style.left = 0;
    leftSide.style.top = 0;
    leftSide.style.width = splitter.offsetLeft - splitter.offsetWidth / 2 + 'px';

    rightSide.style.left = (splitter.offsetLeft + splitter.offsetWidth / 2) + 'px';
    rightSide.style.top = 0;
    rightSide.style.width = container.offsetWidth - splitter.offsetLeft - 10 +  'px';
    
    container.appendChild(leftSide);
    container.appendChild(rightSide);
    console.log('left of right pane is ', rightSide.offsetLeft)
    let mouseIsDown = false;
    let startX = null;
    let globalXCoordinate = null;

    // Will not touch
    splitter.addEventListener('mousedown', function(evt) {
        evt.preventDefault();
        mouseIsDown = true;
        startX = evt.offsetX;
        startY = evt.offsetY;
    });

    leftSide.addEventListener('mousemove', function(evt) {
        evt.preventDefault();
        let left = this.offsetLeft;
        globalXCoordinate = left + evt.offsetX - startX;
    });

    rightSide.addEventListener('mousemove', function(evt) {
        evt.preventDefault();
        let left = this.offsetLeft;
        globalXCoordinate = left + evt.offsetX - startX;
    });

    splitter.addEventListener('mousemove', function(evt) {
        evt.preventDefault();
        let left = this.offsetLeft;
        globalXCoordinate = left + evt.offsetX - startX;
    });


    document.body.addEventListener('mouseup', function(evt) {
        mouseIsDown = false;
    });

    document.addEventListener('mouseup', function(evt) {
        mouseIsDown = false;
    });


    document.addEventListener('mousemove', function(evt) {
        evt.preventDefault();
        evt.stopPropagation();

        let containerWidth = container.getBoundingClientRect().width;
        let hoveringOnDocument = evt.target.nodeName == 'HTML' || evt.target.nodeName == 'BODY';
        let docX = evt.offsetX - container.getBoundingClientRect().x - startX;
        if (mouseIsDown) {


            console.log(splitter.offsetWidth);
            // When dragging what do we need to do to take care of inner splitter areas?

            if (hoveringOnDocument) {
                if (docX < 0) {
                    docX = 0;
                }
                    
                if (docX + splitter.offsetWidth > container.offsetWidth) {
                    docX = containerWidth - splitter.offsetWidth;
                }
                
                splitter.style.left = docX + 'px';
                leftSide.style.width = splitter.offsetLeft - splitter.offsetWidth / 2 + 'px';
                rightSide.style.width = (container.offsetWidth - leftSide.offsetWidth - splitter.offsetWidth) + 'px';
                rightSide.style.left = splitter.offsetLeft + (splitter.offsetWidth / 2) + 'px';
            } else {
                if (globalXCoordinate + splitter.offsetWidth > containerWidth) {
                    globalXCoordinate = containerWidth - splitter.offsetWidth;
                }

                if (globalXCoordinate < 0) {
                    globalXCoordinate = 0;
                }

                splitter.style.left = globalXCoordinate + 'px';
                leftSide.style.width = splitter.offsetLeft - splitter.offsetWidth / 2 + 'px';
                rightSide.style.width = (container.offsetWidth - leftSide.offsetWidth - splitter.offsetWidth) + 'px';
                
                
                rightSide.style.left = splitter.offsetLeft + splitter.offsetWidth / 2 + 'px';
            }
        }
    });
};


const VSplitterBar = function (container, topContent, bottomContent) {
    // We want two divs that we're dividing
    const topSide = document.createElement('div');
    const bottomSide = document.createElement('div');
    const splitter = document.createElement('div');

    topSide.classList.add('topSide');
    bottomSide.classList.add('bottomSide');
    splitter.classList.add('splitter');

    if (topContent !== null) {
        topSide.appendChild(topContent);
    }

    if (bottomContent !== null) {
        bottomSide.appendChild(bottomContent);
    }


    container.appendChild(splitter);

    splitter.style.height = '10px';
    splitter.style.top = '50%';
    splitter.style.transform = 'translateY(-50%)';

    splitter.style.background = 'gray';
    splitter.style.cursor = 'row-resize';


    topSide.style.top = 0;
    topSide.style.left = 0;
    topSide.style.height = splitter.offsetTop - splitter.offsetHeight / 2 + 'px';

    bottomSide.style.top = (splitter.offsetTop + splitter.offsetHeight / 2) + 'px';
    bottomSide.style.left = 0;
    bottomSide.style.height = container.offsetHeight - splitter.offsetTop - 10 + 'px';

    container.appendChild(topSide);
    container.appendChild(bottomSide);
    console.log('top of bottom pane is ', bottomSide.offsetTop)
    let mouseIsDown = false;
    let startY = null;    
    let globalYCoordinate = null;

    // Will not touch
    splitter.addEventListener('mousedown', function (evt) {
        evt.preventDefault();
        mouseIsDown = true;
        startX = evt.offsetX;
        startY = evt.offsetY;
    });

    topSide.addEventListener('mousemove', function (evt) {
        evt.preventDefault();
        let top = this.offsetTop;
        globalYCoordinate = top + evt.offsetY - startY;
    });

    bottomSide.addEventListener('mousemove', function (evt) {
        evt.preventDefault();
        let top = this.offsetTop;
        globalYCoordinate = top + evt.offsetY - startY;
    });

    splitter.addEventListener('mousemove', function (evt) {
        evt.preventDefault();
        let top = this.offsetTop;
        globalYCoordinate = top + evt.offsetY - startY;
    });


    document.body.addEventListener('mouseup', function (evt) {
        mouseIsDown = false;
    });

    document.addEventListener('mouseup', function (evt) {
        mouseIsDown = false;
    });


    document.addEventListener('mousemove', function (evt) {
        evt.preventDefault();
        evt.stopPropagation();

        let containerHeight = container.getBoundingClientRect().height;
        let hoveringOnDocument = evt.target.nodeName == 'HTML' || evt.target.nodeName == 'BODY';
        let docY = evt.offsetY - container.getBoundingClientRect().y - startY;
        if (mouseIsDown) {


            console.log(splitter.offsetHeight);
            // When dragging what do we need to do to take care of inner splitter areas?

            if (hoveringOnDocument) {
                if (docY < 0) {
                    docY = 0;
                }

                if (docY + splitter.offsetHeight> container.offsetHeight) {
                    docY = containerHeight - splitter.offsetHeight;
                }

                splitter.style.top = docY + 'px';
                topSide.style.Height = splitter.offsetTop - splitter.offsetHeight / 2 + 'px';
                bottomSide.style.Height = (container.offsetHeight - topSide.offsetHeight - splitter.offsetHeight) + 'px';
                bottomSide.style.top = splitter.offsetTop + (splitter.offsetHeight / 2) + 'px';
            } else {
                if (globalYCoordinate + splitter.offsetHeight > containerHeight) {
                    globalYCoordinate = containerHeight - splitter.offsetHeight;
                }

                if (globalYCoordinate < 0) {
                    globalYCoordinate = 0;
                }

                splitter.style.top = globalYCoordinate + 'px';
                topSide.style.Height = splitter.offsetTop - splitter.offsetHeight / 2 + 'px';
                bottomSide.style.Height = (container.offsetHeight - topSide.offsetHeight - splitter.offsetHeight) + 'px';


                bottomSide.style.top = splitter.offsetTop + splitter.offsetHeight / 2 + 'px';
            }
        }
    });
};



