

  // Global vars
  var dataChannelLabel = "testchannel";
  var reliableSocket = null;
  var peerConnection = null;
  var dataChannel = null;
  var remoteCandidates = [];
  var have_answer = false;

  function reliable_log_msg(msg) {
    console.log(msg);
    $("#reliable_log_list").prepend("<li>" + msg + "</li>");
  }

  function datachannel_log_msg(msg) {
    console.log(msg);
    $("#datachannel_log_list").prepend("<li>" + msg + "</li>");
  }

    $(document).ready(function () {

      var sourceBuffer = null;
      var video = document.querySelector('video');
      var mimeCodec = 'video/mp4; codecs="avc1.42E01E, mp4a.40.2"';

      if ('MediaSource' in window && MediaSource.isTypeSupported(mimeCodec)) {
        var mediaSource = new MediaSource;
        video.src = URL.createObjectURL(mediaSource);
        mediaSource.addEventListener('sourceopen', function () {
          sourceBuffer = mediaSource.addSourceBuffer(mimeCodec);
          sourceBuffer.addEventListener('updateend', function () {
            video.play();
          });
        });
      } else {
        console.error("Unsupported MIME type or codec: ", mimeCodec);
      }

      /**
       * Add the various callback handlers to the PeerConnection.
       * Shared between both clients.
       */
      var setupPeerConnection = function () {
        peerConnection = new RTCPeerConnection({
          iceServers: [{
              urls: [
                "stun:stun.l.google.com:19302",
                "stun:stun1.l.google.com:19302",
                "stun:stun2.l.google.com:19302",
                "stun:stun3.l.google.com:19302",
                "stun:stun4.l.google.com:19302"
        ]}]});

        peerConnection.onicecandidate = function (event) {
          if (event.candidate) {
            reliableSocket.sendMessage("candidate", event.candidate);
          } else {
            datachannel_log_msg("All local candidates received");
          }
        };

        peerConnection.ondatachannel = function (event) {
          if (event.channel.label == dataChannelLabel) {
            dataChannel = event.channel;
            datachannel_log_msg("DataChannel received");
            setupDataChannel(event.channel);
          } else {
            datachannel_log_msg("Unknown CataChannel label: " + event.channel.label);
          }
        }
      };

      /**
       * Add the various callback handlers to the DataChannel.
       * Shared between both clients.
       */
      var setupDataChannel = function (dataChannel) {
        dataChannel.onopen = function (e) {
          datachannel_log_msg("DataChannel open and ready to be used");

          $("#send_datachannel_msg").click(function () {
            var msg = $("#datachannel_msg").val();
            datachannel_log_msg("Sending message: " + msg);
            dataChannel.send(msg);
          });
        };

        dataChannel.onclose = function () {
          datachannel_log_msg("DataChannel closed");
        };

        dataChannel.onerror = function (e) {
          datachannel_log_msg("DataChannel error: " + e.message);
          console.log(e);
        };

        dataChannel.onmessage = function (e) {
          datachannel_log_msg("Received message: " + e.data);
          if (sourceBuffer != null) {
            sourceBuffer.appendBuffer(e.data);
          } else {
            console.log("Got data but sourceBuffer is null");
          }
        };
      };

      var createOffer = function () {
        setupPeerConnection();
        dataChannel = peerConnection.createDataChannel("testchannel");
        setupDataChannel(dataChannel);

        peerConnection.createOffer().then(function(offer) {
          return peerConnection.setLocalDescription(offer);
        })
        .then(function() {
          reliableSocket.sendMessage("offer", peerConnection.localDescription);
        })
        .catch(function(reason) {
          // An error occurred, so handle the failure to connect
          console.log("RTC Error", reason);
        });
      };

      var createAnswer = function (msg) {
        setupPeerConnection();

        var desc = new RTCSessionDescription(msg);

        peerConnection.setRemoteDescription(desc)
        .then(function () {
          return peerConnection.createAnswer();
        })
        .then(function(answer) {
          return peerConnection.setLocalDescription(answer);
        })
        .then(function() {
          reliableSocket.sendMessage("answer", peerConnection.localDescription);
        })
        .catch(function () {
          console.log("RTC Error", reason);
        });
      };

      var handleCandidate = function (msg) {
        var candidate = new RTCIceCandidate(msg);
        peerConnection.addIceCandidate(candidate).then(function () {
          datachannel_log_msg("New remote candidate received");
        }).catch(function (e) {
          console.log("Error: Failure during addIceCandidate()", e);
        });
      }

      $("#connect_channel").on('click', function (e) {
       // var channel_name = $("#channel_name").val();
        //var wsAddress = "ws://" + window.location.host + "/channel/" + channel_name;
       // console.log("Attempting WebSocket connection to " + wsAddress);

        //reliableSocket = new WebSocket(wsAddress);

        window.WebSocket = window.WebSocket || window.MozWebSocket;

        if (!window.WebSocket) {
          alert('Your browser doesn\'t support WebSocket');
          return;
        }

        reliableSocket = new WebSocket(window.location.href.replace('http://', 'ws://').replace('https://', 'wss://'));




        reliableSocket.onopen = function (event) {
          // Socket is now ready to send and receive messages
          console.log("reliableSocket is open and ready to use");
          reliableSocket.sendMessage("client_connected", {});
        };

        reliableSocket.onerror = function (event) {
          // Socket failed to connect
        };

        reliableSocket.onclose = function (event) {
          console.log("ERROR: Reliable socket has closed");
        };

        // Simple helper to send JSON messages with a given type
        reliableSocket.sendMessage = function (type, msg) {
          reliable_log_msg("Sending msg of type: " + type);
          reliableSocket.send(JSON.stringify({"type": type, "msg": msg}));
        }

        reliableSocket.onmessage = function (event) {
          console.log("Got msg", event);
          var msg = JSON.parse(event.data);

          reliable_log_msg("Received msg of type: " + msg.type);
          console.log(msg);

          switch (msg.type) {
            case "client_connected":
              reliable_log_msg("Client connected: starting RTC handshake");
              createOffer();
              break;
            case "client_disconnected":
              reliable_log_msg("Remote client disconnected");
              break;
            case "offer":
              createAnswer(msg.msg);
              break;
            case "answer":
              peerConnection.setRemoteDescription(new RTCSessionDescription(msg.msg))
              .then(function () {
                have_answer = true;
                var i = 0;
                for (i = 0; i < remoteCandidates.length; i++) {
                  handleCandidate(remoteCandidates[i]);
                }
              });
              break;
            case "candidate":
              if (msg.msg.candidate) {
                if (!have_answer) {
                  remoteCandidates.push(msg.msg);
                } else {
                  handleCandidate(msg.msg);
                }
              } else {
                console.log("Remote peer has no more candidates");
              }
              break;
            default:
              console.log("WARNING: Ignoring unknown msg of type '" + msg.type + "'");
              break;
          }
        };


      });

    });
