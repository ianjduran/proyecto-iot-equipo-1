const mysql = require('../database/db')

class MainController {

    async logIntento(req , res){
        console.log(req.params.mascotaID)
        console.log(req.params.puedeAlimentar)
        if(req.params.mascotaID != null && req.params.puedeAlimentar != null) {
            let mascotaID = decodeURI(req.params.mascotaID);
            console.log(req.params.mascotaID)
            let puedeAlimentar = req.params.puedeAlimentar;
            //let deviceID = req.params.deviceID
            //let temperature = req.params.temperature;
            var sql = `CALL SPIntento('${mascotaID}',${puedeAlimentar})`
            mysql.query(sql, (error,data,fields) => {
                if(error) {
                    res.status(500)
                    res.send(error.message)
                } else {
                    console.log(data)
                    res.json({
                        status: 200,
                        message: "Log uploaded successfully",
                        affectedRows: data.affectedRows
                    })
                }
            })
        } else {
          res.send('Por favor llena todos los datos!')
        }
    }
}

const intentoController = new MainController()
module.exports = intentoController;