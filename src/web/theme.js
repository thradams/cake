/* Light/dark theme: saved choice, else system preference */
(function ()
{
    function savedTheme()
    {
        try
        {
            return localStorage.getItem("cake-theme");
        }
        catch (e)
        {
            return null;
        }
    }

    function systemTheme()
    {
        return window.matchMedia && window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light";
    }

    function applyTheme(theme)
    {
        document.documentElement.setAttribute("data-theme", theme);
        var button = document.getElementById("theme-toggle");
        if (button)
        {
            button.innerText = theme === "dark" ? "☀ Light" : "☾ Dark";
        }
        document.dispatchEvent(new CustomEvent("cake-theme", { detail: theme }));
    }

    window.cakeTheme = function ()
    {
        return document.documentElement.getAttribute("data-theme") || "light";
    };

    applyTheme(savedTheme() || systemTheme());

    document.addEventListener("DOMContentLoaded", function ()
    {
        /* pages may place their own #theme-toggle, otherwise it floats top-right */
        var button = document.getElementById("theme-toggle");
        if (!button)
        {
            button = document.createElement("button");
            button.id = "theme-toggle";
            button.className = "theme-toggle-floating";
            document.body.appendChild(button);
        }
        button.title = "Toggle light/dark theme";
        button.addEventListener("click", function ()
        {
            var theme = window.cakeTheme() === "dark" ? "light" : "dark";
            try
            {
                localStorage.setItem("cake-theme", theme);
            }
            catch (e)
            {
            }
            applyTheme(theme);
        });
        applyTheme(window.cakeTheme());
    });
})();
