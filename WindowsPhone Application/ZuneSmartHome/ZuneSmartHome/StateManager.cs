using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZuneSmartHome
{

    /// <summary>
    /// Class to control the state of the Arduino
    /// </summary>
    class StateManager
    {
        /// <summary>
        /// The red led state 
        /// </summary>
        public bool Quarto { get; set; }

        /// <summary>
        /// The green led state 
        /// </summary>
        public bool Sala { get; set; }

        /// <summary>
        /// Initialize the state manager.
        /// </summary>
        public void Initialize()
        {
            Quarto = false;
            Sala = false;
        }
    }
}
