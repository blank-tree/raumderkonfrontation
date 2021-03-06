$(function () {

    // Statemachine:
    // 0 - Welcome & Language Selection
    // 1 - Description
    // 2 - Choose Date
    // 3 - Choose Time
    // 4 - Paying
    // 5 - Checkout

    // Var
    var timeoutIdle = 600000; // 10min
    var timeoutRegular = 60000; // 1min
    var timeoutPaying = 120000; // 2min
    var timeoutCheckout = 40000; // 40sec
    var fadeTime = 400; // 0.5sec
    var price = 1000; // CHF 10.-

    var currentTimeout;
    var currentState = 0;
    var previousState = 0;
    var moneyCollected = 0.0;
    var language = '';
    var appointmentDate = '';
    var appointmentTime = '';
    var $blocking = $('#blocking');
    var $payment = $('.payment');
    var $date = $('.checkout-date');
    var $time = $('.checkout-time');
    var $end = $('.checkout-endtime');

    // $('body, html').on('touchstart touchmove', function(e) {
    //     e.preventDefault();
    // });

    // shitr.io Connection
    var client = mqtt.connect('mqtt://b23695cf:36a044b175c04e97@broker.shiftr.io', {
        clientId: 'RdK-iPad'
    });

    client.on('connect', function () {
        console.log('client has connected!');
        client.subscribe('/moneyCollected');
    });

    client.on('message', function (topic, message) {
        console.log('new message:', topic, message.toString());
        if (currentState == 4 && topic == '/moneyCollected') {
            moneyCollected = parseFloat(message.toString());
            console.log('money collected: ' + moneyCollected);
            $payment.text(moneyCollected <= price ? ((price - moneyCollected) / 100).toFixed(2) : 0.00);
            resetTimeout(timeoutPaying);
            checkPayment();
        }
    });

    // Choose Language
    $('#screen-0 .button').click(function () {
        language = $(this).data('language');
        $('.content-' + language).show();
        client.publish('/language', language);
        changeState(true, timeoutRegular);
    });

    // Choose Date
    $('#screen-2 .date-button').click(function () {
        if (!$(this).hasClass('disabled')) {
            $('#screen-2 .date-button').removeClass('primary').addClass('hollow');
            $(this).removeClass('hollow').addClass('primary');
            $('#screen-3 .time-button').removeClass('primary').addClass('hollow');
            $('#screen-3 .forward-button').addClass('disabled');
            appointmentDate = $(this).data('date');
            var appointmentDateMessage = appointmentDate <=9 ? "0" + appointmentDate.toString() : appointmentDate.toString();
            client.publish('/appointmentDate', appointmentDateMessage);
            $date.text(appointmentDateMessage);
            $('#screen-2 .forward-button').removeClass('disabled');
            $('#screen-3 .timetables').hide();
            $('#screen-3 #date-' + appointmentDate).show();
        }
    });

    // Choose Time
    $('#screen-3 .time-button').click(function () {
        if (!$(this).hasClass('disabled')) {
            $('#screen-3 .time-button').removeClass('primary').addClass('hollow');
            $(this).removeClass('hollow').addClass('primary');
            appointmentTime = $(this).data('time');
            var appointmentTimeMessage = appointmentTime <=9 ? "0" + appointmentTime.toString() : appointmentTime.toString();
            client.publish('/appointmentTime', appointmentTimeMessage);
            var endTimeMessage = appointmentTime + 1 <= 9 ? "0" + (appointmentTime + 1) : appointmentTime + 1;
            $time.text(appointmentTimeMessage);
            $end.text(endTimeMessage);
            $('#screen-3 .forward-button').removeClass('disabled');
        }
    });

    $('.back-button').click(function () {
        $payment.text((price / 100).toFixed(2));
        changeState(false, timeoutRegular);
    });

    $('.forward-button').click(function () {
		if (!$(this).hasClass('disabled')) {
            changeState(true, timeoutRegular);
        }
    });

    $('#pay').click(function() {
        moneyCollected = price;
        resetTimeout(timeoutPaying);
        checkPayment();
    });

    function checkPayment() {
        if (moneyCollected >= price) {
            client.publish('/print', '1');
            changeState(true, timeoutCheckout);
        }
    }

    function resetTVM() {
        client.publish('/resettvm', '1');
        $('body').fadeTo(fadeTime, 0, function () {
            if (currentState != 5) {
                location.reload();
            } else {
                window.location.href = "/booking.php?date=" + appointmentDate + "&time=" + appointmentTime;
            }
        });
    }

    function resetTimeout(timeoutTime) {
        if (currentState != 4) {
            pauseScreen();
        }
        clearTimeout(currentTimeout);
        if (currentState != 0) {
            currentTimeout = setTimeout(resetTVM, timeoutTime);
        } else {
            currentTimeout = setTimeout(resetTVM, timeoutIdle);
        }
    }

    function changeState(direction, timeoutTime) {
        previousState = currentState;
        if (direction) {
            currentState++;
        } else {
            currentState--;
        }

        $('#screen-' + previousState).css('opacity', 0);
        setTimeout(function() {
            $('#screen-' + previousState).hide();
            $('#screen-' + currentState).show();
        }, fadeTime);
        setTimeout(function() {
            $('#screen-' + currentState).css('opacity', 1);
        }, fadeTime + 100);

        if (currentState === 0) {
            resetTVM();
        } else {
            resetTimeout(timeoutTime);
        }

        console.log('changed from state ' + previousState + ' to state ' + currentState);
    }


    function pauseScreen() {
        console.log('blocking active');
        $blocking.show();
        setTimeout(unpauseScreen, fadeTime * 2);
    }

    function unpauseScreen() {
        console.log('blocking inactive');
        $blocking.hide();
    }


    resetTimeout(timeoutIdle);
    $('#screen-0').css("opacity", 1);
    $payment.text((price / 100).toFixed(2));

});
