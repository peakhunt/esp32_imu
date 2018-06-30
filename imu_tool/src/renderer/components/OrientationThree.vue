<template>
  <div :width="width" :height="height" ref="threeCanvas" class="absolute_container">
    <v-btn fixed @click="set_heading_offset()" style="position: absolute; top: 20px; left: 20px;" color="primary">
    Reset Heading

    <div style="position: absolute; top: 40px; left: 20px; z-index: 100; display: block;" color="primary">Roll : {{roll_fixed}}</div>
    <div style="position: absolute; top: 60px; left: 20px; z-index: 100; display: block;" color="primary">Pitch : {{pitch_fixed}}</div>
    <div style="position: absolute; top: 80px; left: 20px; z-index: 100; display: block;" color="primary">Yaw : {{yaw_fixed}}</div>
    </v-btn>
  </div>
</template>

<script>
  const THREE = require('three')
  // const OrbitControls = require('three-orbit-controls')(THREE)

  export default {
    name: 'OrientationThree',
    props: ['width', 'height', 'orientation'],
    computed: {
      roll_fixed () {
        return this.r.roll.toFixed(2)
      },
      pitch_fixed () {
        return this.r.pitch.toFixed(2)
      },
      yaw_fixed () {
        return this.r.yaw.toFixed(2)
      }
    },
    methods: {
      do_render () {
        this.renderer.render(this.scene, this.camera)
        requestAnimationFrame(this.do_render)
      },
      set_heading_offset () {
        this.heading_offset = -this.r.yaw

        console.log('setting heading offset to:' + this.heading_offset)

        this.set_orientation(this.r)
      },
      set_orientation (o) {
        // FIXME. change the code to use quaternion to avoid gimbal lock
        // from euler angles.

        this.r.roll = o.roll
        this.r.pitch = o.pitch
        this.r.yaw = o.yaw

        var roll = o.roll
        var pitch = o.pitch
        var yaw = o.yaw + this.heading_offset

        var z = roll * Math.PI / 180
        var x = pitch * Math.PI / 180
        var y = yaw * Math.PI / 180

        var rot = new THREE.Euler(-x, -y, z, 'XYZ')

        this.cube.setRotationFromEuler(rot)

        var arrowXDir = new THREE.Vector3(0, 0, -1)
        var arrowYDir = new THREE.Vector3(1, 0, 0)
        var arrowZDir = new THREE.Vector3(0, 1, 0)

        arrowXDir.applyEuler(rot)
        arrowYDir.applyEuler(rot)
        arrowZDir.applyEuler(rot)

        arrowXDir.normalize()
        arrowYDir.normalize()
        arrowZDir.normalize()

        this.arrowX.setDirection(arrowXDir)
        this.arrowY.setDirection(arrowYDir)
        this.arrowZ.setDirection(arrowZDir)
      }
    },
    mounted () {
      var w = this.$refs.threeCanvas.offsetWidth
      var h = this.$refs.threeCanvas.offsetHeight

      console.log('w: ' + w + ',' + 'h: ' + h)

      this.camera = new THREE.PerspectiveCamera(75, this.width / this.height, 0.1, 1000)
      this.camera.position.set(0, 0, 10)

      this.renderer = new THREE.WebGLRenderer({
        alpha: false
      })
      this.renderer.setSize(this.width, this.height)
      this.$refs.threeCanvas.appendChild(this.renderer.domElement)
      // this.renderer.setClearColor(new THREE.Color(0xffffff, 1.0))

      // FIXME resize handling!

      this.scene = new THREE.Scene()

      this.scene.add(new THREE.AmbientLight(0x7f0000))

      this.light = new THREE.PointLight(0xaa7a40, 2, 1500)
      this.light.position.set(-100, 200, 100)
      this.scene.add(this.light)

      var geometry = new THREE.BoxGeometry(3, 0.5, 5)
      // var material = new THREE.MeshBasicMaterial({color: 0xa3a3a3})
      var material = new THREE.MeshPhongMaterial({color: 0xff6383})
      this.cube = new THREE.Mesh(geometry, material)

      this.scene.add(this.camera)
      this.scene.add(this.cube)

      // this.axisHelper = new THREE.AxisHelper(3.25)
      // this.scene.add(this.axisHelper)

      var origin = new THREE.Vector3(0, 0, 0)

      var arrowXDir = new THREE.Vector3(0, 0, -1)
      var arrowYDir = new THREE.Vector3(1, 0, 0)
      var arrowZDir = new THREE.Vector3(0, 1, 0)

      this.arrowX = new THREE.ArrowHelper(arrowXDir, origin, 5, 0xff0000)
      this.arrowY = new THREE.ArrowHelper(arrowYDir, origin, 3, 0x00ff00)
      this.arrowZ = new THREE.ArrowHelper(arrowZDir, origin, 2.5, 0x0000ff)

      this.scene.add(this.arrowX)
      this.scene.add(this.arrowY)
      this.scene.add(this.arrowZ)

      // this.controls = new OrbitControls(this.camera, this.renderer.domElement)

      this.do_render()
    },
    beforeDestroy () {
      this.camera = null
      this.renderer = null
      this.scene = null
      this.light = null
      this.cube = null
      this.controls = null
      this.axisHelper = null
      this.arrowX = null
      this.arrowY = null
      this.arrowZ = null
    },
    data () {
      return {
        camera: null,
        renderer: null,
        scene: null,
        light: null,
        cube: null,
        controls: null,
        axisHelper: null,
        arrowX: null,
        arrowY: null,
        arrowZ: null,
        heading_offset: 0,
        r: {
          roll: 0,
          pitch: 0,
          yaw: 0
        }
      }
    },
    watch: {
      orientation (no, oo) {
        this.set_orientation(no)
      }
    }
  }
</script>

<style scoped>
.absolute_container {
  position: relative;
  float: left;
  left: 50%;
  transform: translateX(-50%);
}
</style>
