const SplitterBar = function (container, leftContent, rightContent, is_vertical) {

    const leftSide = document.createElement('div');
    const rightSide = document.createElement('div');
    const splitter = document.createElement('div');

    if (leftContent !== null) {
        leftSide.appendChild(leftContent);
    }

    if (rightContent !== null) {
        rightSide.appendChild(rightContent);
    }

    container.appendChild(splitter);

    if (is_vertical) {
    
        leftSide.classList.add('leftSide');
        rightSide.classList.add('rightSide');
        splitter.classList.add('splitter');

        splitter.style.position = 'absolute';
        splitter.style.height = '100%';
        //splitter.style.overflow = 'hidden';
        splitter.style.zIndex  = 1;

        splitter.style.width = '5px';
        splitter.style.left = '50%';
        splitter.style.transform = 'translateX(-50%)';

        splitter.style.background = 'RGB(244,244,244)';
        splitter.style.border = "1px solid #d2d3d4";

        splitter.style.cursor = 'ew-resize';//'col-resize';
        
        leftSide.style.position = 'absolute';
        leftSide.style.left = 0;
        leftSide.style.top = 0;
        leftSide.style.width = splitter.offsetLeft - splitter.offsetWidth / 2 + 'px';
        leftSide.style.height = "100%";

        rightSide.style.position = 'absolute';
        rightSide.style.left = (splitter.offsetLeft + splitter.offsetWidth / 2) + 'px';
        rightSide.style.top = 0;
        rightSide.style.width = container.offsetWidth - splitter.offsetLeft - 10 + 'px';
        rightSide.style.height = "100%";
    }
    else {

        leftSide.classList.add('topSide');
        rightSide.classList.add('bottomSide');
        splitter.classList.add('splitter');

        splitter.style.height = '5px';
        splitter.style.top = '80%';
        splitter.style.transform = 'translateY(-50%)';
        splitter.style.zIndex  = 1;
        splitter.style.background = 'RGB(244,244,244)';
        splitter.style.border = "1px solid #d2d3d4";
        splitter.style.cursor = 'ns-resize'; //'row-resize';


        leftSide.style.top = 0;
        leftSide.style.left = 0;
        leftSide.style.height = splitter.offsetTop - splitter.offsetHeight / 2 + 'px';

        rightSide.style.top = (splitter.offsetTop + splitter.offsetHeight / 2) + 'px';
        rightSide.style.left = 0;
        rightSide.style.height = container.offsetHeight - splitter.offsetTop - 10 + 'px';

    }


    container.appendChild(leftSide);
    container.appendChild(rightSide);

    let mouseIsDown = false;

    let splitter_start = 0;

    splitter.addEventListener('mousedown', function (evt) {
        evt.preventDefault();
        mouseIsDown = true;

        if (is_vertical) {
            startX = evt.screenX;
            splitter_start = splitter.offsetLeft;
        }
        else {
            startX = evt.screenY;
            splitter_start = splitter.offsetTop;
        }
    });

    document.addEventListener('mouseup', function (evt) {

        if (is_vertical) {
            let containerWidth = container.getBoundingClientRect().width;

            leftSide.style.width = splitter.offsetLeft - splitter.offsetWidth / 2 + 'px';
            rightSide.style.width = (container.offsetWidth - leftSide.offsetWidth - splitter.offsetWidth) + 'px';
            rightSide.style.left = splitter.offsetLeft + splitter.offsetWidth / 2 + 'px';
        }
        else {
            let containerHeight = container.getBoundingClientRect().height;

            leftSide.style.height = splitter.offsetTop - splitter.offsetHeight / 2 + 'px';
            rightSide.style.height = (container.offsetHeight - leftSide.offsetHeight - splitter.offsetHeight) + 'px';
            rightSide.style.top = splitter.offsetTop + splitter.offsetHeight / 2 + 'px';
        }
        mouseIsDown = false;
    }, true);

    document.addEventListener('mousemove', function (evt) {
        if (mouseIsDown) {

            if (is_vertical) {
                let delta = evt.screenX - startX;
                splitter.style.left = splitter_start + delta + 'px';
            }
            else {
                let delta = evt.screenY - startX;
                splitter.style.top = splitter_start + delta + 'px';
            }
        }
    }, true);
};

