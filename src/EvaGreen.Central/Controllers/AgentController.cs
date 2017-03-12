using System.Linq;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using EvaGreen.Common;

namespace EvaGreen.Central.Controllers
{
    [Authorize]
    [Route("api/[controller]")]
    public sealed class AgentController : Controller
    {
        [HttpGet]
        public IActionResult Get()
        {
            using (var con = new DataConnection())
            {
                return Json(con.AgentConfiguration.ToList());
            }
        }
    }
}
