var avWebformEmbeded = avWebformEmbeded || (function () {
    var display, type, height, width, position, event, src, parent, iframe, popup, conditions, hash, delay = null;
    var content = document.createElement("div");
    content.id = 'av-we-content';

    return {
      init: function (Args) {
        display = Args.display;
        type = Args.type;
        height = Args.height == 'default' ? '90%' : Args.height;
        width = Args.width == 'default' ? '90%' : Args.width;
        position = Args.position;
        event = Args.event;
        src = Args.src;
        conditions = JSON.parse(Args.conditions);
        hash = Args.hash;
        delay = Args.delay;

        content.className = 'av-we-position-' + position + ' av-we-display-' + display;
        content.style.width = width;
        content.style.height = height;
        if (type == 'iframe') {
          iframe = document.createElement("iframe");
          iframe.src = src;
          iframe.id = 'av-we-iframe'
          content.appendChild(iframe);
        }
        else {
          content.innerHTML = src;
        }

        if (display == 'popup') {
          popup = {
            overlay: document.createElement("div"),
            close: document.createElement("div"),
            content: null
          };
          popup.overlay.id = 'av-we-overlay';
          popup.close.id = 'av-we-close';
          popup.close.innerHTML = 'X';
          popup.content = content;
          popup.content.insertBefore(popup.close, popup.content.firstChild);
        }
        else {
          parent = document.getElementsByName('av-we-init')[0].parentElement;
        }
      },
      show: function () {
        if (!avWebformEmbeded.applyConditions()) return;
        switch (event) {
          case 'onload':
            avWebformEmbeded.addElems();
            break;

          case 'onscroll':
          case 'onbottom':
            var onload = window.innerHeight + window.scrollY;
            var executed = false;
            document.addEventListener('scroll', function (e) {
              if (!executed) {
                var current = window.innerHeight + window.scrollY;
                if (((event == 'onbottom' && current >= document.documentElement.scrollHeight)) || (event == 'onscroll' && onload != current)) {
                  e.target.removeEventListener(e.type, arguments.callee);
                  executed = true;
                  avWebformEmbeded.addElems();
                }
              }
            }, true);
            break;

          case 'onexit':
            executed = false;
            document.addEventListener("mouseout", function (e) {
              if (!executed) {
                e = e ? e : window.event;
                var from = e.relatedTarget || e.toElement;
                if (!from || from.nodeName == "HTML") {
                  executed = true;
                  avWebformEmbeded.addElems();
                }
              }
            });
            break;

          case 'delay':
            setTimeout(function() {
                avWebformEmbeded.addElems();
              }, delay * 1000);
            break;
        }
      },
      close: function () {
        document.getElementById('av-we-overlay').remove();
        document.getElementById('av-we-content').remove();
      },
      addElems: function () {
        if (display == 'popup') {
          document.body.insertBefore(popup.content, document.body.firstChild);
          document.body.insertBefore(popup.overlay, document.body.firstChild);
          document.getElementById('av-we-overlay').addEventListener('click', function (e) {
            avWebformEmbeded.close();
          });

          document.getElementById('av-we-close').addEventListener('click', function (e) {
            avWebformEmbeded.close();
          });
        }
        else {
          parent.insertBefore(content, parent.firstChild);
        }
      },
      applyConditions: function () {
        var result = true;
        for (var key in conditions) {
          if (conditions.hasOwnProperty(key)) {
            result = avWebformEmbeded.applyCondition(conditions[key]) && result;
            if (!result) break;
          }
        }

        return result;
      },
      applyCondition: function (condition) {
        var result = false;
        switch (condition.type) {
          case 'referrer':
            if (document.referrer == '') return false;
            switch (condition.condition) {
              case 'contains':
              case 'not_contains':
                result = avWebformEmbeded.applyContains(document.referrer, condition.data, condition.condition);
                break;

              case 'is':
              case 'isnt':
                result = avWebformEmbeded.applyIs(document.referrer, condition.data, condition.condition);
                break;
            }
            break;

          case 'page':
            switch (condition.condition) {
              case 'contains':
              case 'not_contains':
                result = avWebformEmbeded.applyContains(window.location.href, condition.data, condition.condition);
                break;

              case 'is':
              case 'isnt':
                result = avWebformEmbeded.applyIs(window.location.href, condition.data, condition.condition);
                break;
            }
            break;

          case 'agent':
            switch (condition.condition) {
              case 'contains':
              case 'not_contains':
                result = avWebformEmbeded.applyContains(navigator.userAgent, condition.data, condition.condition);
                break;

              case 'is':
              case 'isnt':
                result = avWebformEmbeded.applyIs(navigator.userAgent, condition.data, condition.condition);
                break;
            }
            break;

          case 'device':
            result = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
            break;

          case 'frequence':
            switch (condition.condition) {
              case 'always':
                result = true;
                break;

              case 'session':
                var expire = avWebformEmbeded.cookie.read('avWeSessionExpires');
                var count = parseInt(avWebformEmbeded.cookie.read('avWeSessionCount'));
                if (expire == null) {
                  result = true;
                  var date = new Date();
                  date.setTime(date.getTime() + 24 * 3600 * 1000);
                  avWebformEmbeded.cookie.create('avWeSessionExpires', '1 ', date);
                  avWebformEmbeded.cookie.create('avWeSessionCount', 1, date);
                }
                else if (count < condition.data) {
                  result = true;
                  var date = new Date();
                  date.setTime(date.getTime() + 24 * 3600 * 1000);
                  avWebformEmbeded.cookie.create('avWeSessionCount', count + 1, date);
                }
                else {
                  return false;
                }
                break;
              case 'once':
                var once = avWebformEmbeded.cookie.read('avWeOnce');
                if (once == null) {
                  result = true;
                  var date = new Date();
                  date.setTime(date.getTime() + 365 * 24 * 3600 * 1000);
                  avWebformEmbeded.cookie.create('avWeOnce', '1', date);
                }
                else {
                  return false;
                }
                break;

              case 'max':
                var max = parseInt(avWebformEmbeded.cookie.read('avWeMax'));
                if (max == null) {
                  result = true;
                  var date = new Date();
                  date.setTime(date.getTime() + 365 * 24 * 3600 * 1000);
                  avWebformEmbeded.cookie.create('avWeOnce', 1, date);
                }
                else if (max < condition.data) {
                  result = true;
                  var date = new Date();
                  date.setTime(date.getTime() + 365 * 24 * 3600 * 1000);
                  avWebformEmbeded.cookie.create('avWeOnce', count + 1, date);
                }
                else {
                  return false;
                }
                break;

              case 'once_per':
                var oncePer = avWebformEmbeded.cookie.read('avWeOncePer');
                if (oncePer == null) {
                  result = true;
                  var date = new Date();
                  date.setTime(date.getTime() + condition.data * 60 * 1000);
                  avWebformEmbeded.cookie.create('avWeOncePer', '1', date);
                }
                else {
                  return false;
                }
                break;
            }
            break;
        }

        return result;
      },
      applyContains: function (input, pattern, op) {
        var result = input.match(new RegExp(pattern));
        result = result != null && result.length > 0;
        return op == 'not_contains' ? !result : result;
      },
      applyIs: function (input, pattern, op) {
        var result = input == pattern;
        return op == 'isnt' ? !result : result;
      },
      cookie: {
        create: function (name, value, date) {
          name = name + hash;
          var expires = '';
          if (date) {
            expires = "; expires=" + date.toUTCString();
          }
          document.cookie = name + "=" + value + expires + "; path=/";
        },

        read: function (name) {
          name = name + hash;
          var nameEQ = name + "=";
          var ca = document.cookie.split(';');
          for (var i = 0; i < ca.length; i++) {
            var c = ca[i];
            while (c.charAt(0) == ' ') c = c.substring(1, c.length);
            if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
          }
          return null;
        },
        erase: function (name) {
          name = name + hash;
          createCookie(name, "", -1);
        }
      }
    };
  }());
