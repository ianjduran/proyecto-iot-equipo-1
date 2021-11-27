const express = require('express');
const intentoController = require('../controllers/intentoController')
const router = express.Router();

router.post('/api/experimentos_web_api/:mascotaID/:puedeAlimentar', intentoController.logIntento);

module.exports = router;