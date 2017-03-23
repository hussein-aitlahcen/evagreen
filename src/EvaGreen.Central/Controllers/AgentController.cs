using System.Linq;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using EvaGreen.Common;
using System;

namespace EvaGreen.Central.Controllers
{
    [Authorize]
    [Route("api/[controller]")]
    public sealed class AgentController : Controller
    {
        [HttpGet]
        public IActionResult GetAll()
        {
            using (var con = new DataConnection())
            {
                return Json(con.AgentConfiguration.ToList());
            }
        }

        [HttpGet("{id}", Name = "GetAgent")]
        public IActionResult GetById(int id)
        {
            using (var con = new DataConnection())
            {
                return Json(con.AgentConfiguration.FirstOrDefault(a => a.Id == id));
            }
        }

        [HttpPut("{id}")]
        public IActionResult Update(int id, [FromBody] AgentConfiguration configuration)
        {
            try
            {
                using (var con = new DataConnection())
                {
                    con.Update(configuration);
                    return Json(new
                    {
                        Message = string.Empty,
                        State = 1,
                    });
                }
            }
            catch (Exception e)
            {
                return Json(new
                {
                    Message = e.Message,
                    State = 2
                });
            }
        }
    }
}
