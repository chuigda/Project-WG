import platform, sys

uname = platform.uname()

About_Text = """ --- ABOUT DUMMY SYSTEM ---

Dummy system is capable of tracking, replaying and automatically generating
VTS model control parameters, in order to achieve semi-auto "piloting".

The Dummy System is a part of the Project-WG, developed by 7th Design Inc.
It is licensed under the GNU Affero General Public License v3.0. See the
LICENSE file for more information.

 --- SYSTEM INFORMATION ---

OS_NAME = %s
OS_VERSION = %s
OS_ARCH = %s
PYTHON_VERSION = %s
""" % (
    uname.system,
    uname.version,
    uname.machine,
    sys.version.split(" ")[0]
)
