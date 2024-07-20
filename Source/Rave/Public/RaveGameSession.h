

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"
#include "RaveGameSession.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API ARaveGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	ARaveGameSession();

	bool HostSession(TWeakObjectPtr<APlayerController> PlayerController, int32 PublicConnections, bool bUseLAN);
	bool FindSessions(TWeakObjectPtr<APlayerController> PlayerController, int32 MaxResults, bool bUseLAN);
	bool JoinSession(TWeakObjectPtr<APlayerController> PlayerController, const FOnlineSessionSearchResult& SearchResult);
	bool QuickJoinSession(TWeakObjectPtr<APlayerController> PlayerController, bool bUseLAN);
	bool QuickJoinFindSession(TWeakObjectPtr<APlayerController> PlayerController, bool bUseLAN);
	bool StartSession();
	bool EndSession();
	bool DestroySession(TWeakObjectPtr<APlayerController> PlayerController);

	/**
	* Get the search results found and the current search result being probed
	*
	* @param SearchResultIdx idx of current search result accessed
	* @param NumSearchResults number of total search results found in FindGame()
	*
	* @return State of search result query
	*/
	EOnlineAsyncTaskState::Type GetSearchResultStatus(int32& NumSearchResults);

	TArray<FOnlineSessionSearchResult>& GetSessionSearchResults() const;

public:
	/** Default number of players allowed in a game */
	static const int32 DEFAULT_NUM_PLAYERS = 6;

protected:
	/** Current session search settings */
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearch;

	// Delegate for session

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnQuickJoinSessionFindCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	// Delegate fired when a session request has completed

	void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnQuickJoinSessionFindComplete(bool bWasSuccessful);
	void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);

	// Handles to various registered delegates

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnQuickJoinSessionFindCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
};
