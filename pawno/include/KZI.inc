//------------------------------------------------------------------------------
// SA:MP PROJECT: KOWAZ ZONE INCLUDE (KZI)
// Author: Bence "Kowalski" Fejes 2008
// Website: http://www.kowaz.hu
//
// COPYRIGHT (c) KowaZ 2007-2008
//------------------------------------------------------------------------------


/*----------------------------------------------------------------------------*-
native KZI_Init();
native KZI_OnPlayerSpawn( playerid );
native KZI_OnPlayerDisconnect( playerid, reason );
native KZI_Loop(); // ZONE_LOOP_FREUQENCY
native
native Zone:Zone_Add( name[], Float:minx, Float:miny, Float:maxx, Float:maxy, color );
native Zone_Remove( Zone:zoneid );
native Zone_Attack( Zone:zoneid, Gang:attacker );
native Zone_CancelAttack( Zone:zoneid );
native Zone_IsActive( Zone:zoneid );
native Zone_IsLocked( Zone:zoneid );
native Zone_IsVisible( Zone:zoneid );
native Zone_IsCapturable( Zone:zoneid );
native Zone_IsFlashing( Zone:zoneid );
native Zone_ToggleLocked( Zone:zoneid, toggle );
native Zone_ToggleCapturable( Zone:zoneid, toggle );
native Zone_ToggleVisible( Zone:zoneid, toggle );
native Zone_ToggleFlashing( Zone:zoneid, toggle );
native Zone_GetPos( Zone:zoneid, &Float:minx, &Float:miny, &Float:maxx, &Float:maxy );
native Zone_GetName( Zone:zoneid, name[], length );
native Zone_GetColor( Zone:zoneid );
native Zone_GetOwner( Zone:zoneid );
native Zone_GetAttacker( Zone:zoneid );
native Zone_GetGangZone( Zone:zoneid );
native Zone_SetPos( Zone:zoneid, Float:minx, Float:miny, Float:maxx, Float:maxy );
native Zone_SetName( Zone:zoneid, name[] );
native Zone_SetColor( Zone:zoneid, newcolor );
native Zone_SetOwner( Zone:zoneid, Gang:newowner );
native Zone_ReturnName( Zone:zoneid );
native Zone_Recreate( Zone:zoneid );
native
native Gang:Gang_Add( name[], color, visible );
native Gang_Remove( Gang:gangid );
native Gang_IsActive( Gang:gangid );
native Gang_IsVisible( Gang:gangid );
native Gang_ToggleVisible( Gang:gangid, toggle );
native Gang_GetColor( Gang:gangid );
native Gang_GetName( Gang:gangid, name[], length );
native Gang_SetColor( Gang:gangid, newcolor );
native Gang_SetName( Gang:gangid, newname[] );
native Gang_ReturnName( Gang:gangid );
native
native Zone:Player_GetZone( playerid );
native Player_InInZone( playerid, Zone:zoneid );
native Player_IsInAnyZone( playerid );
native Gang:layer_GetGang( playerid );
native Player_InInGang( playerid, Gang:gangid );
native Player_IsInAnyGang( playerid );
native Player_PutInGang( playerid, Gang:gangid );
native Player_RemoveFromGang( playerid );
native Player_SetToGangColor( playerid );
-*----------------------------------------------------------------------------*/



//------------------------------------------------------------------------------
// Including the main SA:MP functions
//------------------------------------------------------------------------------
#include <a_samp>


//------------------------------------------------------------------------------
// Constans #define directives
//------------------------------------------------------------------------------
#define PLAYER_ZONE_ARRAY_SIZE (sizeof(g_pKowaZ_Player_Flags))
#define PLAYER_ZONE_SET(%1,%2) (g_pKowaZ_Player_Flags[(%1)]&=~PLAYER_FLAG_ZONE,g_pKowaZ_Player_Flags[(%1)]|=(_:(%2))&PLAYER_FLAG_ZONE)
#define PLAYER_ZONE_GET(%1) (Zone:(g_pKowaZ_Player_Flags[(%1)]&PLAYER_FLAG_ZONE))

#define PLAYER_GANG_ARRAY_SIZE (sizeof(g_pKowaZ_Player_Flags))
#define PLAYER_GANG_SET(%1,%2) (g_pKowaZ_Player_Flags[(%1)]&=~PLAYER_FLAG_GANG,g_pKowaZ_Player_Flags[(%1)]|=(_:(%2)<<8)&PLAYER_FLAG_GANG)
#define PLAYER_GANG_GET(%1) (Gang:((g_pKowaZ_Player_Flags[(%1)]&PLAYER_FLAG_GANG)>>8))

#define PLAYER_FLAG_ZONE (255)
#define PLAYER_FLAG_GANG (256+512+1024+2048)

#define MAX_ZONES (255)
#define MAX_ZONE_NAME (27)
#define MAX_GANGS (15)
#define MAX_GANG_NAME (27)

#define GANG_FLAG_COLOR (0xffffff)
#define GANG_FLAG_ACTIVE (16777216)
#define GANG_FLAG_VISIBLE (33554432)

#define GANG_LIMIT (Gang:MAX_GANGS)
#define GANG_INVALID (Gang:MAX_GANGS)

#define ZONE_LIMIT (Zone:MAX_ZONES)
#define ZONE_INVALID (Zone:MAX_ZONES)

#define ZONE_LOOP_FREQUENCY (300)

#define ZONE_FLAG_ACTIVE (1)
#define ZONE_FLAG_VISIBLE (2)
#define ZONE_FLAG_LOCKED (4)
#define ZONE_FLAG_FLASHING (8)
#define ZONE_FLAG_CAPTURABLE (16)
#define ZONE_FLAG_POINTER (32736)
#define ZONE_FLAG_OWNER (491520)
#define ZONE_FLAG_ATTACKER (7864320)


//------------------------------------------------------------------------------
// Forward declarations (prototypes)
//------------------------------------------------------------------------------
forward OnPlayerEnterZone( playerid, Zone:zoneid );
forward OnPlayerLeaveZone( playerid, Zone:zoneid );


//------------------------------------------------------------------------------
// Our array for the players and the zones
//------------------------------------------------------------------------------
static
    g_pKowaZ_Player_Flags[ MAX_PLAYERS ],
	Float:g_pKZZN_Player_Pos[ MAX_PLAYERS ][ 3 ],
	
	g_pKZZN_Zone_Names[ ZONE_LIMIT ][ MAX_ZONE_NAME ],
	Float:g_pKZZN_Zone_Pos[ ZONE_LIMIT ][ 4 ],
	g_pKZZN_Zone_Flags[ ZONE_LIMIT ],
	g_pKZZN_Zone_Color[ ZONE_LIMIT ],
	
	g_pKZGG_Gang_Names[ GANG_LIMIT ][ MAX_GANG_NAME ],
	g_pKZGG_Gang_Flags[ GANG_LIMIT ];


//------------------------------------------------------------------------------
// NAME: Zone_IsActive
// DESC: Returns the activity of the given zoneid.
//------------------------------------------------------------------------------
stock Zone_IsActive( Zone:zoneid )
{
	if( zoneid < Zone:0 && zoneid >= ZONE_LIMIT ) return 0;
	return g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_ACTIVE;
}


//------------------------------------------------------------------------------
// NAME: Zone_GetGangZone
// DESC: Returns the gangzone of the given zone.
//------------------------------------------------------------------------------
stock Zone_GetGangZone( Zone:zoneid ){
	return ((g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_POINTER) >> 5);
}


//------------------------------------------------------------------------------
// NAME: Zone_CancelAttack
// DESC: Cancels the current gangwar on the zone.
//------------------------------------------------------------------------------
stock Zone_CancelAttack( Zone:zoneid ){
	g_pKZZN_Zone_Flags[ zoneid ] |= ZONE_FLAG_ATTACKER;
}


//------------------------------------------------------------------------------
// NAME: Zone_GetColor
// DESC: Returns the color of the given zone.
//------------------------------------------------------------------------------
stock Zone_GetColor( Zone:zoneid ){
	return g_pKZZN_Zone_Color[ zoneid ];
}


//------------------------------------------------------------------------------
// NAME: Zone_GetOwner
// DESC: Returns the owner of the zone.
//------------------------------------------------------------------------------
stock Gang:Zone_GetOwner( Zone:zoneid ){
	return (Gang:((g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_OWNER) >> 15));
}


//------------------------------------------------------------------------------
// NAME: Zone_GetAttacker
// DESC: Returns the attacker gang's id of the zone.
//------------------------------------------------------------------------------
stock Gang:Zone_GetAttacker( Zone:zoneid ){
	return (Gang:((g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_ATTACKER) >> 19));
}


//------------------------------------------------------------------------------
// NAME: Zone_SetOwner
// DESC: Sets the new owner of a zone.
//------------------------------------------------------------------------------
stock Zone_SetOwner( Zone:zoneid, Gang:newowner )
{
	g_pKZZN_Zone_Flags[ zoneid ] &= ~(ZONE_FLAG_OWNER|ZONE_FLAG_ATTACKER);
	g_pKZZN_Zone_Flags[ zoneid ] |= (_:newowner<<15)&ZONE_FLAG_OWNER|ZONE_FLAG_ATTACKER;
}


//------------------------------------------------------------------------------
// NAME: Zone_Attack
// DESC: Makes the zone attacked by the given gang.
//------------------------------------------------------------------------------
stock Zone_Attack( Zone:zoneid, Gang:attacker )
{
	if( Zone_GetOwner( zoneid ) != GANG_INVALID ){
		g_pKZZN_Zone_Flags[ zoneid ] &= ~ZONE_FLAG_ATTACKER;
		g_pKZZN_Zone_Flags[ zoneid ] |= (_:attacker << 19) & ZONE_FLAG_ATTACKER;
	}
	else
		Zone_SetOwner( zoneid, attacker );
}


//------------------------------------------------------------------------------
// NAME: Zone_IsVisible
// DESC: Returns 1, if the zone is visible.
//------------------------------------------------------------------------------
stock Zone_IsVisible( Zone:zoneid ){
	return g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_VISIBLE;
}


//------------------------------------------------------------------------------
// NAME: Zone_IsFlashing
// DESC: Returns 1, if the zone is flashing at the moment.
//------------------------------------------------------------------------------
stock Zone_IsFlashing( Zone:zoneid ){
	return g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_FLASHING;
}


//------------------------------------------------------------------------------
// NAME: Zone_IsLocked
// DESC: Returns 1, if the zone is locked.
//------------------------------------------------------------------------------
stock Zone_IsLocked( Zone:zoneid ){
	return g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_LOCKED;
}


//------------------------------------------------------------------------------
// NAME: Zone_IsCapturable
// DESC: Returns 1, if the zone can be captured.
//------------------------------------------------------------------------------
stock Zone_IsCapturable( Zone:zoneid ){
	return g_pKZZN_Zone_Flags[ zoneid ] & ZONE_FLAG_CAPTURABLE;
}


//------------------------------------------------------------------------------
// NAME: Player_GetZone
// DESC: Returns the player's currently zone.
//------------------------------------------------------------------------------
stock Zone:Player_GetZone( playerid ){
	return PLAYER_ZONE_GET( playerid );
}


//------------------------------------------------------------------------------
// NAME: Player_IsInZone
// DESC: Returns 1, if the player is in the given.
//------------------------------------------------------------------------------
#define Player_IsInZone(%1,%2)\
	(Player_GetZone(%1)==(%2))


//------------------------------------------------------------------------------
// NAME: Player_IsInAnyZone
// DESC: Returns 1, if the player is in any zone.
//------------------------------------------------------------------------------
#define Player_IsInAnyZone(%1)\
	(Player_GetZone(%1)!=GANG_INVALID)


//------------------------------------------------------------------------------
// NAME: Zone_SetName
// DESC: Changes the zone's name to newname.
//------------------------------------------------------------------------------
stock Zone_SetName( Zone:zoneid, newname[] ){
	strmid( g_pKZZN_Zone_Names[ zoneid ], newname, 0, strlen(newname), MAX_ZONE_NAME );
}


//------------------------------------------------------------------------------
// NAME: Zone_GetName
// DESC: Fill the newname array with the zone's name.
//------------------------------------------------------------------------------
stock Zone_GetName( Zone:zoneid, zname[], zlen ){
	strmid( zname, g_pKZZN_Zone_Names[ zoneid ], 0, strlen(g_pKZZN_Zone_Names[ zoneid ]), zlen );
}


//------------------------------------------------------------------------------
// NAME: Zone_ReturnName
// DESC: Returns the the zone's name.
//------------------------------------------------------------------------------
stock Zone_ReturnName( Zone:zoneid ){
	return g_pKZZN_Zone_Names[ zoneid ];
}


//------------------------------------------------------------------------------
// NAME: Zone_Recreate
// DESC: Recreates the gangzone of the zone.
//------------------------------------------------------------------------------
stock Zone_Recreate( Zone:zoneid )
{
	GangZoneHideForAll( Zone_GetGangZone( zoneid ) );

	GangZoneDestroy( Zone_GetGangZone( zoneid ) );
	g_pKZZN_Zone_Flags[ zoneid ] &= ~ZONE_FLAG_POINTER;
	
	new ptr = GangZoneCreate( g_pKZZN_Zone_Pos[ zoneid ][ 0 ],
                              g_pKZZN_Zone_Pos[ zoneid ][ 1 ],
                              g_pKZZN_Zone_Pos[ zoneid ][ 2 ],
                              g_pKZZN_Zone_Pos[ zoneid ][ 3 ] );
	g_pKZZN_Zone_Flags[ zoneid ] |= (ptr << 5) & ZONE_FLAG_POINTER;

	Zone_ToggleVisible( zoneid, Zone_IsVisible( zoneid ) );
	Zone_ToggleFlashing( zoneid, Zone_IsFlashing( zoneid ) );
}


//------------------------------------------------------------------------------
// NAME: Zone_SetColor
// DESC: Changes the color of the zone.
//------------------------------------------------------------------------------
stock Zone_SetColor( Zone:zoneid, newcolor )
{
	g_pKZZN_Zone_Color[ zoneid ] = newcolor;
	Zone_ToggleVisible( zoneid, Zone_IsVisible( zoneid ) );
	Zone_ToggleFlashing( zoneid, Zone_IsFlashing( zoneid ) );
}


//------------------------------------------------------------------------------
// NAME: Zone_SetPos
// DESC: Sets the poisition of the zone.
//------------------------------------------------------------------------------
stock Zone_SetPos( Zone:zoneid, Float:minx, Float:miny, Float:maxx, Float:maxy )
{
	g_pKZZN_Zone_Pos[ zoneid ][ 0 ] = minx;
	g_pKZZN_Zone_Pos[ zoneid ][ 1 ] = miny;
	g_pKZZN_Zone_Pos[ zoneid ][ 2 ] = maxx;
	g_pKZZN_Zone_Pos[ zoneid ][ 3 ] = maxy;
	
	Zone_Recreate( zoneid );
	Zone_ToggleVisible( zoneid, Zone_IsVisible( zoneid ) );
	Zone_ToggleFlashing( zoneid, Zone_IsFlashing( zoneid ) );
}


//------------------------------------------------------------------------------
// NAME: Zone_GetPos
// DESC: Gets the coordinates of the given zone.
//------------------------------------------------------------------------------
stock Zone_GetPos( Zone:zoneid, &Float:minx, &Float:miny, &Float:maxx, &Float:maxy )
{
	minx = g_pKZZN_Zone_Pos[ zoneid ][ 0 ];
	miny = g_pKZZN_Zone_Pos[ zoneid ][ 1 ];
	maxx = g_pKZZN_Zone_Pos[ zoneid ][ 2 ];
	maxy = g_pKZZN_Zone_Pos[ zoneid ][ 3 ];
}


//------------------------------------------------------------------------------
// NAME: Zone_ToggleLocked
// DESC: Locks or unlocks the given zone.
//------------------------------------------------------------------------------
stock Zone_ToggleLocked( Zone:zoneid, toggle )
{
	if( toggle ) g_pKZZN_Zone_Flags[ zoneid ] |= ZONE_FLAG_LOCKED;
	else g_pKZZN_Zone_Flags[ zoneid ] &= ~ZONE_FLAG_LOCKED;
}


//------------------------------------------------------------------------------
// NAME: Zone_ToggleCapturable
// DESC: Make a zone uncapturable or capturable for gangs.
//------------------------------------------------------------------------------
stock Zone_ToggleCapturable( Zone:zoneid, toggle )
{
	if( toggle ) g_pKZZN_Zone_Flags[ zoneid ] |= ZONE_FLAG_CAPTURABLE;
	else g_pKZZN_Zone_Flags[ zoneid ] &= ~ZONE_FLAG_CAPTURABLE;
}


//------------------------------------------------------------------------------
// NAME: Zone_ToggleVisible
// DESC: Make the zone visible or invisible.
//------------------------------------------------------------------------------
stock Zone_ToggleVisible( Zone:zoneid, toggle )
{
	if( toggle ){
	    GangZoneShowForAll( Zone_GetGangZone( zoneid ), Zone_GetColor( zoneid ) );
		g_pKZZN_Zone_Flags[ zoneid ] |= ZONE_FLAG_VISIBLE;
	}
	else {
	    GangZoneHideForAll( Zone_GetGangZone( zoneid ) );
		g_pKZZN_Zone_Flags[ zoneid ] &= ~ZONE_FLAG_VISIBLE;
	}
}


//------------------------------------------------------------------------------
// NAME: Zone_ToggleFlashing
// DESC: Set the zone flashing or stops the flash.
//------------------------------------------------------------------------------
stock Zone_ToggleFlashing( Zone:zoneid, toggle )
{
	if( toggle ){
	    GangZoneFlashForAll( Zone_GetGangZone( zoneid ), 0 );
		g_pKZZN_Zone_Flags[ zoneid ] |= ZONE_FLAG_FLASHING;
	}
	else {
	    GangZoneStopFlashForAll( Zone_GetGangZone( zoneid ) );
		g_pKZZN_Zone_Flags[ zoneid ] &= ~ZONE_FLAG_FLASHING;
	}
}


//------------------------------------------------------------------------------
// NAME: Zone_Add
// DESC: Adds a new zone to the server.
//------------------------------------------------------------------------------
stock Zone:Zone_Add( zname[], Float:minx, Float:miny, Float:maxx, Float:maxy, color )
{
	new Zone:i;
	for( i = Zone:0; i < ZONE_LIMIT; i++ )
	    if( !(g_pKZZN_Zone_Flags[ i ] & ZONE_FLAG_ACTIVE) ) break;
	if( i == ZONE_LIMIT ) return ZONE_INVALID;
	new ptr = GangZoneCreate( minx, miny, maxx, maxy );
	if( ptr == INVALID_GANG_ZONE ) return ZONE_INVALID;
	g_pKZZN_Zone_Flags[ i ] = ZONE_FLAG_ACTIVE|ZONE_FLAG_VISIBLE|ZONE_FLAG_CAPTURABLE;
	g_pKZZN_Zone_Flags[ i ] |= ((ptr << 5) & ZONE_FLAG_POINTER)|ZONE_FLAG_ATTACKER|ZONE_FLAG_OWNER;
	g_pKZZN_Zone_Pos[ i ][ 0 ] = minx;
	g_pKZZN_Zone_Pos[ i ][ 1 ] = miny;
	g_pKZZN_Zone_Pos[ i ][ 2 ] = maxx;
	g_pKZZN_Zone_Pos[ i ][ 3 ] = maxy;
	GangZoneShowForAll( ptr, color );
	strmid( g_pKZZN_Zone_Names[ i ], zname, 0, strlen(zname), MAX_ZONE_NAME );
	g_pKZZN_Zone_Color[ i ] = color;
	return i;
}


//------------------------------------------------------------------------------
// NAME: Zone_Remove
// DESC: Deletes the zone from the system.
//------------------------------------------------------------------------------
stock Zone_Remove( Zone:zoneid )
{
	GangZoneHideForAll( Zone_GetGangZone( zoneid ) );
	GangZoneDestroy( Zone_GetGangZone( zoneid ) );
	g_pKZZN_Zone_Flags[ zoneid ] &= ~ZONE_FLAG_ACTIVE;
}


//------------------------------------------------------------------------------
// NAME: IsXYInZone
// DESC: Returns 1, if the given 2D coordinates are in the given zone.
//------------------------------------------------------------------------------
stock IsXYInZone( Float:x, Float:y, Zone:zoneid )
{
	if( x < g_pKZZN_Zone_Pos[ zoneid ][ 2 ] && x >= g_pKZZN_Zone_Pos[ zoneid ][ 0 ] && y < g_pKZZN_Zone_Pos[ zoneid ][ 3 ] && y >= g_pKZZN_Zone_Pos[ zoneid ][ 1 ] ) return 1;
	return 0;
}


//------------------------------------------------------------------------------
// NAME: KZI_Init
// DESC: Installs the KZI system. Paste ths entry point of your script!
//------------------------------------------------------------------------------
KZI_Init()
{
	for( new i; i < PLAYER_ZONE_ARRAY_SIZE; i++ )
		PLAYER_ZONE_SET( i, ZONE_INVALID );

	for( new i; i < PLAYER_GANG_ARRAY_SIZE; i++ )
		PLAYER_GANG_SET( i, GANG_INVALID );
}


//------------------------------------------------------------------------------
// NAME: KZI_OnPlayerSpawn
// DESC: Paste this in the OnPlayerSpawn() callback to get the system working!
//------------------------------------------------------------------------------
KZI_OnPlayerSpawn( playerid )
{
	new Float: x,
		Float: y,
		Float: z;

	GetPlayerPos( playerid, x, y, z );
	PLAYER_ZONE_SET( playerid, ZONE_INVALID );

	for( new Zone:index = Zone:0; index < ZONE_LIMIT; index++ ){
		if( Zone_IsActive( index ) ){
			if( Zone_IsVisible( index ) ){
				GangZoneShowForPlayer( playerid, Zone_GetGangZone( index ), Zone_GetColor( index ) );
				if( Zone_IsFlashing( index ) )
					GangZoneFlashForPlayer( playerid, Zone_GetGangZone( index ), 0x00 );
			}
			if( IsXYInZone( x, y, Zone: index ) ){
				PLAYER_ZONE_SET( playerid, index );
				CallRemoteFunction( "OnPlayerEnterZone", "ii", playerid, _:index );
			}
		}
	}
}


//------------------------------------------------------------------------------
// NAME: KZI_Loop
// DESC: The engine of the KZI system. Call it in every 300ms.
//------------------------------------------------------------------------------
KZI_Loop()
{
	for( new i = 0; i < PLAYER_ZONE_ARRAY_SIZE; i++ ){
		if( !IsPlayerConnected( i ) || GetPlayerState( i ) == 0 || GetPlayerState( i ) == 9 || GetPlayerState( i ) == 8 || GetPlayerState( i ) == 7 ){
			PLAYER_ZONE_SET( i, ZONE_INVALID );
		}
		else {
			new Float:x, Float:y, Float:z,
				Zone:zoneid = Zone:PLAYER_ZONE_GET( i );

			GetPlayerPos( i, x, y, z );
			if( zoneid != ZONE_INVALID ){
				if( IsXYInZone( x, y, zoneid ) ){
					g_pKZZN_Player_Pos[ i ][ 0 ] = x;
					g_pKZZN_Player_Pos[ i ][ 1 ] = y;
					g_pKZZN_Player_Pos[ i ][ 2 ] = z;
					continue;
				}
				PLAYER_ZONE_SET( i, ZONE_INVALID );
				CallRemoteFunction( "OnPlayerLeaveZone", "ii", i, _:zoneid );
			}
			for( zoneid = Zone:0; zoneid < ZONE_LIMIT; zoneid++ ){
				if( Zone_IsActive( zoneid ) && IsXYInZone( x, y, zoneid ) ){
					CallRemoteFunction( "OnPlayerEnterZone", "ii", i, _:zoneid );
					if( Zone_IsLocked( zoneid ) ){
						new Float: a;
						if( !IsPlayerInAnyVehicle( i ) ){
							GetPlayerFacingAngle( i, a );
							SetPlayerPos( i, g_pKZZN_Player_Pos[ i ][ 0 ] - 3.0 * floatsin( -a, degrees ), g_pKZZN_Player_Pos[ i ][ 1 ] - 1.0 * floatcos( -a, degrees ), g_pKZZN_Player_Pos[ i ][ 2 ] + 1.5 );
							SetPlayerFacingAngle( i, a - 180.0 );
						}
						else {
							GetVehicleZAngle( i, a );
							SetVehiclePos( GetPlayerVehicleID( i ), g_pKZZN_Player_Pos[ i ][ 0 ] - 3.0 * floatsin( -a, degrees ), g_pKZZN_Player_Pos[ i ][ 1 ] - 1.0 * floatcos( -a, degrees ), g_pKZZN_Player_Pos[ i ][ 2 ] + 1.5 );
							SetVehicleZAngle( GetPlayerVehicleID( i ), a - 180.0 );
						}
						break;
					}
					PLAYER_ZONE_SET( i, zoneid );
					break;
				}
			}
			g_pKZZN_Player_Pos[ i ][ 0 ] = x;
			g_pKZZN_Player_Pos[ i ][ 1 ] = y;
			g_pKZZN_Player_Pos[ i ][ 2 ] = z;
		}
	}
}


//------------------------------------------------------------------------------
// NAME: Gang_IsActive
// DESC: Checks the activity of the gang.
//------------------------------------------------------------------------------
stock Gang_IsActive( Gang:gangid ){
	return (g_pKZGG_Gang_Flags[ gangid ] & GANG_FLAG_ACTIVE);
}


//------------------------------------------------------------------------------
// NAME: Gang_ReturnName
// DESC: Returns the name of the given gang.
//------------------------------------------------------------------------------
stock Gang_ReturnName( Gang:gangid ){
	return g_pKZGG_Gang_Names[ gangid ];
}


//------------------------------------------------------------------------------
// NAME: Gang_SetName
// DESC: Changes the gang's name.
//------------------------------------------------------------------------------
stock Gang_SetName( Gang:gangid, zname[] ){
	strmid( g_pKZGG_Gang_Names[ gangid ], zname, 0, strlen(name), MAX_GANG_NAME );
}


//------------------------------------------------------------------------------
// NAME: Gang_GetName
// DESC: Pastes the gang' name in the given array.
//------------------------------------------------------------------------------
stock Gang_GetName( Gang:gangid, zname[], len ){
	strmid( zname, g_pKZGG_Gang_Names[ gangid ], 0, strlen(g_pKZGG_Gang_Names[ gangid ]), len );
}


//------------------------------------------------------------------------------
// NAME: Gang_Add
// DESC: Creates a new gang with the given data.
//------------------------------------------------------------------------------
stock Gang:Gang_Add( zname[], color, visible )
{
	new Gang:i;
	for( i = Gang:0; i < GANG_LIMIT; i++ )
	    if( !(g_pKZGG_Gang_Flags[ i ] & GANG_FLAG_ACTIVE) ) break;
	if( i == GANG_LIMIT ) return GANG_INVALID;
    g_pKZGG_Gang_Flags[ i ] = (color & GANG_FLAG_COLOR)|GANG_FLAG_ACTIVE|(visible?GANG_FLAG_VISIBLE:0);
	strmid( g_pKZGG_Gang_Names[ i ], zname, 0, strlen(zname), MAX_GANG_NAME );
	return i;
}


//------------------------------------------------------------------------------
// NAME: Gang_Remove
// DESC: Deletes the gang from the system.
//------------------------------------------------------------------------------
stock Gang_Remove( Gang:gangid )
{
	g_pKZGG_Gang_Flags[ gangid ] &= ~GANG_FLAG_ACTIVE;
	for( new i = 0; i < PLAYER_GANG_ARRAY_SIZE )
	    if( PLAYER_GANG_GET( i ) == gangid )
	        PLAYER_GANG_SET( i, GANG_INVALID );
}


//------------------------------------------------------------------------------
// NAME: Gang_GetColor
// DESC: Returns with the gangcolor. (in RGB format!)
//------------------------------------------------------------------------------
stock Gang_GetColor( Gang:gangid ){
	return (g_pKZGG_Gang_Flags[ gangid ] & GANG_FLAG_COLOR);
}


//------------------------------------------------------------------------------
// NAME: Gang_SetColor
// DESC: Sets the gang's zone. (must give in RGB format!)
//------------------------------------------------------------------------------
stock Gang_SetColor( Gang:gangid, newcolor )
{
	g_pKZGG_Gang_Flags[ gangid ] &= ~GANG_FLAG_COLOR;
	g_pKZGG_Gang_Flags[ gangid ] |= newcolor & GANG_FLAG_COLOR;
}


//------------------------------------------------------------------------------
// NAME: Gang_IsVisible
// DESC: Returns 1, if the gang's color is visible.
//------------------------------------------------------------------------------
stock Gang_IsVisible( Gang:gangid ){
	return (g_pKZGG_Gang_Flags[ gangid ] & GANG_FLAG_VISIBLE);
}


//------------------------------------------------------------------------------
// NAME: Gang_ToggleVisible
// DESC: Sets the gang's color's alpha to 0 or 255.
//------------------------------------------------------------------------------
stock Gang_ToggleVisible( Gang:gangid, toggle )
{
	if( !toggle ) g_pKZGG_Gang_Flags[ gangid ] &= ~GANG_FLAG_VISIBLE;
	else g_pKZGG_Gang_Flags[ gangid ] |= GANG_FLAG_VISIBLE;
}


//------------------------------------------------------------------------------
// NAME: Player_PutInGang
// DESC: Puts the player in the given gang.
//------------------------------------------------------------------------------
stock Player_PutInGang( playerid, Gang:gangid ){
	PLAYER_GANG_SET( playerid, gangid );
}


//------------------------------------------------------------------------------
// NAME: Player_RemoveFromGang
// DESC: Kicks the player from the gang.
//------------------------------------------------------------------------------
stock Player_RemoveFromGang( playerid ){
	PLAYER_GANG_SET( playerid, GANG_INVALID );
}


//------------------------------------------------------------------------------
// NAME: Player_GetGang
// DESC: Returns the player's gang.
//------------------------------------------------------------------------------
stock Gang:Player_GetGang( playerid ){
	return PLAYER_GANG_GET( playerid );
}


//------------------------------------------------------------------------------
// NAME: Player_IsInGang
// DESC: Returns 1, if the player is in the given gang.
//------------------------------------------------------------------------------
#define Player_IsInGang(%1,%2)\
	(Player_GetGang(%1)==(%2))


//------------------------------------------------------------------------------
// NAME: Player_IsInAnyGang
// DESC: Returns 1, if the player is in any gang.
//------------------------------------------------------------------------------
#define Player_IsInAnyGang(%1)\
	(Player_GetGang(%1)!=GANG_INVALID)


//------------------------------------------------------------------------------
// NAME: Player_SetToGangColor
// DESC: Stest the player's color to the gang's color.
//------------------------------------------------------------------------------
stock Player_SetToGangColor( playerid )
{
	if( PLAYER_GANG_GET( playerid ) == GANG_INVALID ) return;
	SetPlayerColor( playerid, (Gang_GetColor( PLAYER_GANG_GET( playerid ) ) << 8)|(Gang_IsVisible( PLAYER_GANG_GET( playerid ) )?0xff:0x00) );
}


//------------------------------------------------------------------------------
// NAME: KZI_OnPlayerDisconnect
// DESC: Paste this in the OnPlayerDisconnect callback!
//------------------------------------------------------------------------------
KZI_OnPlayerDisconnect( playerid, reason )
{
	PLAYER_GANG_SET( playerid, GANG_INVALID );
#pragma unused reason
}

//------------------------------------------------------------------------------
// COPYRIGHT (c) KowaZ 2007-2008
//------------------------------------------------------------------------------
