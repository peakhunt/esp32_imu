import Vue from 'vue'

export const ImuComm = new Vue({
  methods: {
    getIMUData () {
      var self = this
      var url = 'http://' + self.ipAddress + ':' + self.port + '/imu/orientation'

      self.numRequest++
      self.$http.get(url)
        .then((response) => {
          // console.log('got data')
          self.numSuccess++
          if (self.isStopped === true) {
            return
          }
          self.$emit('imuOrientation', response.data)
          if (self.wait !== 0) {
            self.timer = setTimeout(() => {
              self.getIMUData()
            }, self.wait)
          } else {
            self.getIMUData()
          }
        }, (err) => {
          self.numFail++
          console.log('failed to retrieve:' + err)
          self.isStopped = true
        })
    },
    start (serverInfo) {
      this.ipAddress = serverInfo.ipAddress
      this.port = serverInfo.port
      this.wait = serverInfo.wait
      this.bufferSize = serverInfo.bufferSize
      this.isStopped = false

      this.$emit('onConnected')

      console.log('start polling: ' + this.ipAddress + ':' + this.port)
      this.getIMUData()
    },
    stop () {
      console.log('stop polling: ' + this.ipAddress + ':' + this.port)

      this.$emit('onDisconnected')
      this.isStopped = true
      if (this.timer != null) {
        clearTimeout(this.timer)
        this.timer = null
      }
    }
  },
  data: {
    ipAddress: null,
    port: null,
    wait: 0,
    isStopped: true,
    timer: null,
    bufferSize: 5,
    numRequest: 0,
    numSuccess: 0,
    numFail: 0
  }
})
