$(document).ready(function() {
    $('li', '#navbar').click(function() {
        $(this).addClass('active').siblings().removeClass('active');
    });

    $('#mobile-menu').click(function(event) {
        event.preventDefault();

        $('.menu').slideToggle();
    });

    $('#scrollToTop').click(function() {
        event.preventDefault();
        window.scrollTo(0, 0);
    });

    (function() {
        if (location.pathname.trim() === '/') {
            $('a[href="/"]').parent().addClass('active');
            return;
        }
        var page = rstrip(location.href, '/').split('/').pop();
        page = decodeURIComponent(page);
        $('a[href*="' + page + '"]', '#navbar').parent().addClass('active');
    })();
});
function rstrip(str, char) {
    var len = str.length,
        i;
    while (str[len-1] == char) {
        len--;
    }
    return str.substring(0, len);
}
