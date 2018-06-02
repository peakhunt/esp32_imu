<template>
  <radial-gauge :value="heading" :options="options"></radial-gauge>
</template>

<script>
  import RadialGauge from 'vue2-canvas-gauges/src/RadialGauge'

  export default {
    name: 'Compass',
    components: {
      RadialGauge
    },
    props: ['value', 'width', 'height'],
    beforeDestroy () {
      if (this.timer != null) {
        clearTimeout(this.timer)
      }
    },
    data () {
      return {
        heading: 0,
        timer: null,
        options: {
          width: this.width,
          height: this.height,
          minValue: 0,
          maxValue: 360,
          majorTicks: [
            'N',
            'NE',
            'E',
            'SE',
            'S',
            'SW',
            'W',
            'NW',
            'N'
          ],
          minorTicks: 22,
          ticksAngle: 360,
          startAngle: 180,
          strokeTicks: false,
          highlights: false,
          colorPlate: '#33a',
          colorMajorTicks: '#f5f5f5',
          colorMinorTicks: '#ddd',
          colorNumbers: '#ccc',
          colorNeedle: 'rgba(240, 128, 128, 1)',
          colorNeedleEnd: 'rgba(255, 160, 122, .9)',
          valueBox: true,
          valueTextShadow: false,
          colorCircleInner: '#fff',
          colorNeedleCircleOuter: '#ccc',
          needleCircleSize: 15,
          needleCircleOuter: false,
          animationRule: 'linear',
          needleType: 'line',
          needleStart: 75,
          needleEnd: 99,
          needleWidth: 3,
          borders: true,
          borderInnerWidth: 0,
          borderMiddleWidth: 0,
          borderOuterWidth: 10,
          colorBorderOuter: '#ccc',
          colorBorderOuterEnd: '#ccc',
          colorNeedleShadowDown: '#222',
          borderShadowWidth: 0,
          animationTarget: 'plate',
          animationDuration: 500,
          animateOnInit: true,
          units: 'ᵍ',
          title: 'Heading',
          fontTitleSize: 19,
          colorTitle: '#f5f5f5'
        }
      }
    },
    watch: {
      value (n, o) {
        var self = this

        if (self.timer === null) {
          self.timer = setTimeout(() => {
            self.heading = n
            self.timer = null
          }, 500)
        }
      }
    }
  }
</script>
