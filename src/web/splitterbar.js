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

    // Append to DOM first so offsetWidth/offsetHeight are real
    container.appendChild(splitter);
    container.appendChild(leftSide);
    container.appendChild(rightSide);

    // Helper: notify all Monaco editors to re-layout
    function notifyEditors() {
        if (typeof inputEditor !== 'undefined' && inputEditor.layout) inputEditor.layout();
        if (typeof outputEditor !== 'undefined' && outputEditor.layout) outputEditor.layout();
    }

    // Helper: recalculate all panel sizes from current splitter position
    function applyLayout() {
        if (is_vertical) {
            const left = splitter.offsetLeft - splitter.offsetWidth / 2;
            leftSide.style.width = left + 'px';
            rightSide.style.width = (container.offsetWidth - left - splitter.offsetWidth) + 'px';
            rightSide.style.left = (splitter.offsetLeft + splitter.offsetWidth / 2) + 'px';
        } else {
            const top = splitter.offsetTop - splitter.offsetHeight / 2;
            leftSide.style.height = top + 'px';
            rightSide.style.height = (container.offsetHeight - top - splitter.offsetHeight) + 'px';
            rightSide.style.top = (splitter.offsetTop + splitter.offsetHeight / 2) + 'px';
        }
        notifyEditors();
    }

    if (is_vertical) {
        leftSide.classList.add('leftSide');
        rightSide.classList.add('rightSide');
        splitter.classList.add('splitter');

        splitter.style.position = 'absolute';
        splitter.style.height = '100%';
        splitter.style.zIndex = 1;
        splitter.style.width = '5px';
        splitter.style.left = '50%';
        splitter.style.transform = 'translateX(-50%)';
        splitter.style.background = 'RGB(244,244,244)';
        splitter.style.border = '1px solid #d2d3d4';
        splitter.style.cursor = 'ew-resize';

        leftSide.style.position = 'absolute';
        leftSide.style.left = '0';
        leftSide.style.top = '0';
        leftSide.style.height = '100%';

        rightSide.style.position = 'absolute';
        rightSide.style.top = '0';
        rightSide.style.height = '100%';

    } else {
        leftSide.classList.add('topSide');
        rightSide.classList.add('bottomSide');
        splitter.classList.add('splitter');

        splitter.style.height = '5px';
        splitter.style.top = '70%';
        splitter.style.transform = 'translateY(-50%)';
        splitter.style.zIndex = 1;
        splitter.style.background = 'RGB(244,244,244)';
        splitter.style.border = '1px solid #d2d3d4';
        splitter.style.cursor = 'ns-resize';

        leftSide.style.top = '0';
        leftSide.style.left = '0';

        rightSide.style.left = '0';
    }

    // Initial layout — elements are in DOM now so offsets are real
    applyLayout();

    // Recompute panel sizes when window is resized,
    // keeping the splitter at the same *ratio* so it doesn't jump to a corner
    window.addEventListener('resize', function () {
        if (is_vertical) {
            // Keep splitter at same fractional position
            const ratio = splitter.offsetLeft / (container.offsetWidth || 1);
            splitter.style.left = (container.offsetWidth * ratio) + 'px';
            splitter.style.transform = ''; // ratio-based, drop the 50% translate
        } else {
            const ratio = splitter.offsetTop / (container.offsetHeight || 1);
            splitter.style.top = (container.offsetHeight * ratio) + 'px';
            splitter.style.transform = '';
        }
        applyLayout();
    });

    let mouseIsDown = false;
    let startX = 0;
    let splitter_start = 0;

    splitter.addEventListener('mousedown', function (evt) {
        evt.preventDefault();
        mouseIsDown = true;

        if (is_vertical) {
            startX = evt.screenX;
            splitter_start = splitter.offsetLeft;
            splitter.style.transform = ''; // switch from % to px positioning
        } else {
            startX = evt.screenY;
            splitter_start = splitter.offsetTop;
            splitter.style.transform = '';
        }
    });

    document.addEventListener('mouseup', function () {
        if (mouseIsDown) {
            mouseIsDown = false;
            applyLayout();
        }
    }, true);

    document.addEventListener('mousemove', function (evt) {
        if (!mouseIsDown) return;

        const minPx = 50; // minimum panel size in pixels

        if (is_vertical) {
            const delta = evt.screenX - startX;
            const raw = splitter_start + delta;
            const clamped = Math.min(
                Math.max(raw, minPx),
                container.offsetWidth - splitter.offsetWidth - minPx
            );
            splitter.style.left = clamped + 'px';
        } else {
            const delta = evt.screenY - startX;
            const raw = splitter_start + delta;
            const clamped = Math.min(
                Math.max(raw, minPx),
                container.offsetHeight - splitter.offsetHeight - minPx
            );
            splitter.style.top = clamped + 'px';
        }

        // Live update panels AND editors while dragging
        applyLayout();
    }, true);
};