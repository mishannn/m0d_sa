/*

	PROJECT:		mod_sa
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright we_sux, FYP

	mod_sa is available from http://code.google.com/p/m0d-s0beit-sa/

	mod_sa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	mod_sa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with mod_sa.  If not, see <http://www.gnu.org/licenses/>.

*/
struct gta_cheat
{
	uint8_t		*addr;
	const char	*name;
};
void	cheat_handle_poscoord( void );
void	cheat_handle_coordmaster( void );
int		cheat_panic ( void );
void	cheat_prevent_freeze ( void );
void	cheat_teleport ( const float pos[3], int interior_id );
void	cheat_teleport_nearest_car ( void );
void	cheat_handle_misc ( void );
void	cheat_handle_debug ( HWND wnd );
void	cheat_handle_spoof_weapon ( void );
void	cheat_handle_weapon_disable ( void );
void	cheat_handle_freeze_vehicles ( struct vehicle_info *vehicle_info, struct actor_info *actor_info );
void	cheat_handle_unlock ( void );
void	cheat_handle_hp ( struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff );
void	cheat_handle_stick ( struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff );
void	cheat_handle_money ( void );
void	cheat_handle_weapon ( void );
void	cheat_handle_teleport ( struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff );
void	cheat_handle_checkpoint ( void );
void	cheat_handle_unfreeze ( struct vehicle_info *vehicle_info, struct actor_info *actor_info );
void	cheat_handle_emo ( struct vehicle_info *vehicle_info, struct actor_info *actor_info, float time_diff );
void	cheat_handle_exit_vehicle ( struct vehicle_info *vehicle_info, struct actor_info *actor_info );
